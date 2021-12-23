// <copyright> 2021 <joey.li>
#ifndef LANGUAGE_H_
#define LANGUAGE_H_
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "include/context.h"

using std::atomic;
using std::function;
using std::map;
using std::mutex;
using std::pair;
using std::set;
using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::vector;

class Language final : public std::enable_shared_from_this<Language> {
 public:
  using Callback = function<void()>;
  Language(const string &name, const ContextPtr &context, const Callback &cb)
      : is_write_file_(true),
        train_count_(0),
        name_(name),
        context_(context),
        cb_(cb) {}
  Language(const string &txt, const ContextPtr &context)
      : is_write_file_(false),
        train_count_(5),
        txt_(txt),
        context_(context),
        cb_(nullptr) {}
  // for Train language
  void Init();
  // for test language txt
  void InitNgrams();
  uint32_t Compare(const shared_ptr<Language> &other);

 private:
  inline bool ReloadGrams();
  inline void TrainGrams();
  inline void WriteFile();
  void TrainGram(const string &txt, int n);
  void Wait_TrainGram(const vector<pair<string, uint32_t>> &grams);

 private:
  bool is_write_file_;
  const Callback cb_;
  string txt_;
  atomic<uint32_t> train_count_;
  mutex train_lock_;
  // language name, such as English
  const string name_;
  const ContextPtr &context_;
  unordered_map<string, uint32_t> n_grams_in_order_;
  vector<pair<string, uint32_t>> sort_grams_;
  map<uint32_t, set<string>> n_grams_;
};
typedef shared_ptr<Language> LanguagePtr;
#endif  // LANGUAGE_H_
