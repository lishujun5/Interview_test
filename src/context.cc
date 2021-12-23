// <copyright> 2021 <joey.li>
#include "include/context.h"

using std::make_shared;

Context::Context(uint32_t thread_size)
    : cfg_(make_shared<Config>()),
      thread_pool_(make_shared<ThreadPool>(thread_size)) {}

bool Context::Init(const string &config_file) {
  return cfg_->Init(config_file);
}
