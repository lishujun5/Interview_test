// <copyright> 2021 <joey.li>
#ifndef CONFIG_H_
#define CONFIG_H_
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

class Config final {
 public:
  struct Content {
    string input_file;
    string output_file;
  };
  bool Init(const string &cfg);
  const unordered_map<string, Content> &profile() const { return profiles_; }
  const Content &GetContext(const string &language_type) {
    return profiles_[language_type];
  }
  string DebugString();

 private:
  // <language type, config content>
  unordered_map<string, Content> profiles_;
};
#endif  // CONFIG_H_
