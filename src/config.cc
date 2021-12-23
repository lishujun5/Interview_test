// <copyright> 2021 <joey.li>
#include "include/config.h"

#include <string.h>

#include <fstream>
#include <sstream>

using std::endl;
using std::ifstream;
using std::stringstream;

bool Config::Init(const string &cfg) {
  static const size_t kLineBytes = 256;
  ifstream ifs;
  ifs.open(cfg, std::ifstream::in);
  char buf[kLineBytes];
  string tmp;
  if (!ifs) {
    return false;
  } else {
    while (ifs.getline(buf, kLineBytes)) {
      tmp.assign(buf, strlen(buf));
      auto first = tmp.find_first_of(' ');
      auto last = tmp.find_last_of(' ');
      if (first == string::npos || last == string::npos) {
        return false;
      }
      profiles_.emplace(
          string(tmp.begin() + last + 1, tmp.end()),
          Config::Content{string(tmp.begin(), tmp.begin() + first),
                          string(tmp.begin() + first + 1, tmp.begin() + last)});
    }
    ifs.close();
    return true;
  }
}

string Config::DebugString() {
  stringstream os;
  for (const auto &iter : profiles_) {
    os << iter.first << ": "
       << "(" << iter.second.input_file << ", " << iter.second.output_file
       << ")" << endl;
  }
  return os.str();
}
