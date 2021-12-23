// <copyright> 2021 <joey.li>
#include "include/language_manager.h"

#include <condition_variable>
#include <iostream>
#include <limits>
#include <memory>

using std::bind;
using std::condition_variable;
using std::make_shared;
using std::numeric_limits;
using std::weak_ptr;

extern condition_variable cv;

void LanguageManager::Init() {
  weak_ptr<LanguageManager> self =
      weak_ptr<LanguageManager>(shared_from_this());
  auto cb = [=]() {
    const auto &language_manager = self.lock();
    if (language_manager) {
      language_manager->Wait_Init();
    }
  };
  for (const auto &iterm : ctx_->cfg()->profile()) {
    auto language = make_shared<Language>(iterm.first, ctx_, cb);
    languages_.emplace(iterm.first, language);
    ctx_->thread_pool()->enqueue(bind(&Language::Init, language));
  }
}

void LanguageManager::Wait_Init() {
  static uint32_t language_finish = 0;
  if (++language_finish == languages_.size()) {
    cv.notify_all();
  }
}

const string LanguageManager::GetLanguageType(const LanguagePtr &other) {
  uint32_t distance = numeric_limits<uint32_t>::max();
  uint32_t cur_distance = 0;
  string language_type;
  for (const auto &language : languages_) {
    cur_distance = language.second->Compare(other);
    if (cur_distance < distance) {
      distance = cur_distance;
      language_type = language.first;
    }
  }
  return language_type;
}