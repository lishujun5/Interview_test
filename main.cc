// <copyright> 2021 <joey.li>
#include <string.h>

#include <condition_variable>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>

#include "include/context.h"
#include "include/language_manager.h"

using std::cout;
using std::cerr;
using std::endl;
using std::shared_ptr;
using std::make_shared;
using std::mutex;
using std::condition_variable;
using std::unique_lock;
using std::ifstream;
using std::ofstream;

condition_variable cv;
mutex test_lock;

int main(int argc, char **argv) {
  bool ret = false;
  if (argc < 2) {
    cerr << "parm error" << endl;
    return -1;
  }

  const string config_file = argv[1];
  ContextPtr context = make_shared<Context>();
  ret = context->Init(config_file);
  if (!ret) {
    cerr << "Init context error" << endl;
    return -1;
  }

  LanguageManagerPtr language_manager = make_shared<LanguageManager>(context);
  language_manager->Init();

  unique_lock<mutex> lock(test_lock);
  cv.wait(lock);

  // if argc is 4, testing is needed
  if (argc == 4) {
    static const uint32_t kLineSize = 256;
    ifstream fs_in;
    ofstream fs_out;
    const string& input_file = argv[2];
    const string& output_file = argv[3];
    fs_in.open(input_file, ifstream::in | ifstream::binary);
    fs_out.open(output_file, ofstream::out | ofstream::binary);
    char buf[kLineSize] = {0};
    string txt;
    while (!fs_in.eof()) {
      memset(buf, 0, kLineSize);
      fs_in.getline(buf, kLineSize);
      txt.assign(buf);
      if (!txt.size()) {
        continue;
      }
      LanguagePtr unknown_language = make_shared<Language>(txt, context);
      unknown_language->InitNgrams();
      fs_out << language_manager->GetLanguageType(unknown_language)
                  << endl;
    }
    fs_in.close();
    fs_out.close();
  }
  return 0;
}
