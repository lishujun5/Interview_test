// <copyright> 2021 <joey.li>
#include "include/language.h"

#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

using std::abs;
using std::bind;
using std::copy;
using std::copy_n;
using std::cout;
using std::endl;
using std::for_each;
using std::fstream;
using std::ifstream;
using std::lock_guard;
using std::sort;
using std::vector;

void Language::Init() {
  struct stat statbuf;
  bool ret = false;
  const auto &content = context_->cfg()->profile().at(name_);
  if (stat(content.output_file.c_str(), &statbuf) || statbuf.st_size == 0) {
    if (stat(content.input_file.c_str(), &statbuf) || statbuf.st_size == 0) {
      cout << content.input_file << "not existed or size is 0";
      return;
    }
    TrainGrams();
  } else {
    ret = ReloadGrams();
    assert(!ret);
    if (cb_) {
      cb_();
    }
  }
}

void Language::InitNgrams() {
  for (auto iter = txt_.begin(); iter != txt_.end(); ++iter) {
    if (isdigit(*iter) || isspace(*iter) || *iter == '\n' || *iter == '\t') {
      *iter = '_';
    }
  }
  TrainGram(txt_, 1);
  TrainGram(txt_, 2);
  TrainGram(txt_, 3);
  TrainGram(txt_, 4);
  TrainGram(txt_, 5);
}

void Language::WriteFile() {
  fstream fs;
  const auto &content = context_->cfg()->profile().at(name_);
  fs.open(content.output_file, fstream::out | fstream::trunc | fstream::binary);
  for (auto iter = n_grams_.crbegin(); iter != n_grams_.crend(); ++iter) {
    for (const auto &str : iter->second)
      fs << str << " " << iter->first << endl;
  }
  fs.close();
}

void Language::TrainGrams() {
  // 1M/Times
  static const size_t kBufSize = 1024 * 1024;
  ifstream fs;
  const auto &content = context_->cfg()->profile().at(name_);
  fs.open(content.input_file, fstream::in);
  char buf[kBufSize] = {0};
  string cur_train_string;
  uint8_t c = -1;
  uint32_t count = -1;
  struct stat stat_buf;
  int rc = stat(content.input_file.c_str(), &stat_buf);
  assert(rc == 0);
  train_count_.store((stat_buf.st_size / kBufSize + 1) * 5);
  while (!fs.eof()) {
    count = 0;
    memset(buf, 0, kBufSize);
    while (!fs.eof() && count < kBufSize) {
      c = fs.get();
      buf[count] = isspace(c) || isdigit(c) || c == '\n' || c == '\t' ? '_' : c;
      ++count;
    }
    cur_train_string.assign(buf);
    context_->thread_pool()->enqueue(
        bind(&Language::TrainGram, shared_from_this(), cur_train_string, 1));
    context_->thread_pool()->enqueue(
        bind(&Language::TrainGram, shared_from_this(), cur_train_string, 2));
    context_->thread_pool()->enqueue(
        bind(&Language::TrainGram, shared_from_this(), cur_train_string, 3));
    context_->thread_pool()->enqueue(
        bind(&Language::TrainGram, shared_from_this(), cur_train_string, 4));
    context_->thread_pool()->enqueue(
        bind(&Language::TrainGram, shared_from_this(), cur_train_string, 5));
  }
}

void Language::TrainGram(const string &txt, int n) {
  unordered_map<string, uint32_t> gram_with_frequency;
  assert(n >= 1 && n <= 5);
  if (!txt.size()) {
    return;
  }
  auto iter = txt.cbegin();
  while (iter + n != txt.cend()) {
    auto &record = ++gram_with_frequency[string(iter, iter + n)];
    ++iter;
  }
  auto cmp = [=](pair<string, uint32_t> left, pair<string, uint32_t> right) {
    return left.second > right.second;
  };
  vector<pair<string, uint32_t>> grams(gram_with_frequency.begin(),
                                       gram_with_frequency.end());
  sort(grams.begin(), grams.end(), cmp);
  if (grams.size() > 400) {
    grams.resize(400);
  }
  Wait_TrainGram(grams);
}

void Language::Wait_TrainGram(const vector<pair<string, uint32_t>> &grams) {
  {
    lock_guard<mutex> g(train_lock_);
    for_each(grams.begin(), grams.end(),
             [=](pair<string, uint32_t> ele) { sort_grams_.push_back(ele); });
  }
  if (--train_count_ != 0) {
    return;
  }
  auto cmp = [=](pair<string, uint32_t> left, pair<string, uint32_t> right) {
    return left.second > right.second;
  };
  sort(sort_grams_.begin(), sort_grams_.end(), cmp);
  if (sort_grams_.size() > 400) {
    sort_grams_.resize(400);
  }
  // init n_grams_with_grequency for testing and n_grams for profile
  set<string> tmp_str;
  if (!sort_grams_.size()) {
    return;
  }
  uint32_t cur_frequency = sort_grams_.begin()->second;
  uint32_t order = 0;
  for (const auto &ele : sort_grams_) {
    n_grams_in_order_.emplace(ele.first, order++);
    auto &str_set = n_grams_[ele.second];
    str_set.insert(ele.first);
  }
  if (is_write_file_) {
    WriteFile();
  }
  if (cb_) {
    cb_();
  }
}

bool Language::ReloadGrams() {
  static const size_t kLineBytes = 256;
  ifstream fs;
  const auto &content = context_->cfg()->profile().at(name_);
  fs.open(content.output_file, ifstream::in | ifstream::binary);
  char buf[kLineBytes];
  string tmp;
  uint32_t order = 0;
  while (!fs.eof()) {
    fs.getline(buf, kLineBytes);
    tmp.assign(buf);
    size_t pos = tmp.find(' ');
    if (pos == string::npos) {
      return false;
    }
    n_grams_in_order_.emplace(string(tmp.begin(), tmp.begin() + pos), order++);
  }
  return true;
}

uint32_t Language::Compare(const LanguagePtr &other) {
  uint32_t sum = 0;
  for (const auto &iterm : other->n_grams_in_order_) {
    if (n_grams_in_order_.find(iterm.first) == n_grams_in_order_.end()) {
      sum += 400;
    } else {
      uint32_t left = iterm.second;
      uint32_t right = other->n_grams_in_order_[iterm.first];
      sum += left > right ? left - right : right - left;
    }
  }
  return sum;
}