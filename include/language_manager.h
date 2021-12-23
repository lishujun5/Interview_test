// <copyright> 2021 <joey.li>
#ifndef LANGUAGE_MANAGER_H_
#define LANGUAGE_MANAGER_H_
#include <memory>
#include <string>
#include <unordered_map>

#include "include/context.h"
#include "include/language.h"

using std::shared_ptr;
using std::string;
using std::unordered_map;

class LanguageManager : public std::enable_shared_from_this<LanguageManager> {
 public:
  explicit LanguageManager(const ContextPtr& ctx) : ctx_(ctx) {}
  void Init();
  void Wait_Init();
  const string GetLanguageType(const LanguagePtr& other);

 private:
  const ContextPtr& ctx_;
  unordered_map<string, LanguagePtr> languages_;
};
typedef shared_ptr<LanguageManager> LanguageManagerPtr;
#endif  // LANGUAGE_MANAGER_H_
