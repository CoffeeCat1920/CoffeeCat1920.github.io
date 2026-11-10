#include "md.hpp"
#include "logger/logger.hpp"
#include "template_map/template_map.hpp"
#include <map>
#include <md4c-html.h>
#include <md4c.h>
#include <regex>
#include <sstream>
#include <string>
#include <utility>

std::map<std::string, std::string> plt_map{
    {"youtube",
     R"(<iframe
        width="560"
        height="315"
        src="https://www.youtube.com/embed/$2">
    </iframe>)"},
};

std::string expand_temp(const std::string &line) {
  std::map<std::string, std::string> &template_maps = get_template_map();
  const std::regex tag(R"re(@include\(\s*([^)]*?)\s*\))re");

  std::smatch match;
  std::string template_str;

  if (std::regex_search(line, match, tag)) {
    std::string name = match[1];
    LOG("FOUND A TEMPLATE: {}", name);
    template_str = template_maps[match[1]];
  }
  return template_str;
}

std::string expand_links(const std::string &line) {
  const std::regex tag(R"re(@([A-Za-z][\w-]*)\(\s*([^)]*?)\s*\))re");

  std::smatch match;
  std::string link;

  if (std::regex_search(line, match, tag)) {
    std::string plt = match[1].str();

    link = std::regex_replace(line, tag, plt_map[plt]);
    return link;
  }

  return line;
}

std::pair<std::string, std::string> exteract_vars(const std::string line) {

  static const std::regex tag(R"(^\+(\w[\w-]*)\s*:\s*(.*?)\s*$)");

  std::smatch match;
  std::string var_line;

  std::regex_search(line, match, tag);
  std::string key = match[1].str();
  std::string val = match[2].str();

  return std::pair<std::string, std::string>{key, val};
}

std::string preprocessor(const std::string &md,
                         std::map<std::string, std::string> &vars_map) {

  std::istringstream stream(md);
  std::string processed;
  std::string line;

  bool skip = false;

  while (std::getline(stream, line)) {

    if (line == "---") {
      skip = !skip;
      continue;
    }

    if (skip) {
      std::pair<std::string, std::string> var = exteract_vars(line);
      vars_map[var.first] = var.second;
      continue;
    }

    processed.append(expand_temp(line));
    processed.append(expand_links(line));
    processed.push_back('\n');
  }

  return processed;
}

static void process_output(const MD_CHAR *text, MD_SIZE size, void *userdata) {
  auto *out = static_cast<std::string *>(userdata);
  out->append(text, size);
}

std::string md_to_html::Convert(std::string md) {
  std::map<std::string, std::string> vars_map;
  std::string processed_md = preprocessor(md, vars_map);
  for (const auto &[a, b] : vars_map) {
    LOG("Key: {}", a);
    LOG("VAL: {}", b);
  }
  std::string html;
  int err =
      md_html(processed_md.c_str(), static_cast<MD_SIZE>(processed_md.size()),
              process_output, &html, MD_DIALECT_GITHUB, MD_HTML_FLAG_DEBUG);
  if (err) {
    LOG_ERROR("Can't convert to html the following file: \n");
  }
  return html;
}
