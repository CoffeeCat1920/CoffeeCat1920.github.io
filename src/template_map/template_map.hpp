#include <map>
#include <string>

inline std::map<std::string, std::string> &get_template_map() {
  static std::map<std::string, std::string> template_map;
  return template_map;
}
