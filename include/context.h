// <copyright> 2021 <joey.li>
#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <memory>
#include <string>

#include "include/config.h"
#include "include/threadpool.h"

using std::shared_ptr;
using std::string;

class Context final {
 public:
  explicit Context(uint32_t thread_size = 4);
  bool Init(const string& config_file);
  const shared_ptr<ThreadPool>& thread_pool() { return thread_pool_; }
  const shared_ptr<Config>& cfg() { return cfg_; }

 private:
  shared_ptr<ThreadPool> thread_pool_;
  shared_ptr<Config> cfg_;
};
typedef shared_ptr<Context> ContextPtr;
#endif  // CONTEXT_H_
