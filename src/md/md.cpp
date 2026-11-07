#include "md.hpp"
#include "logger/logger.hpp"
#include <map>
#include <md4c-html.h>
#include <md4c.h>
#include <regex>
#include <sstream>
#include <string>

std::map<std::string, std::string> plt_map{
    {"yt",
     R"(<iframe
        width="560"
        height="315"
        src="https://www.youtube.com/embed/$2">
    </iframe>)"},
};

std::string expand_links(const std::string &line) {
  const std::regex tag(R"(@(\w+):\s*(.+))");

  std::smatch match;
  std::string link;

  if (std::regex_search(line, match, tag)) {
    std::string plt = match[1].str();

    link = std::regex_replace(line, tag, plt_map[plt]);
    return link;
  }

  return line;
}

std::string preprocessor(const std::string &md) {
  std::istringstream stream(md);
  std::string processed;
  std::string line;

  while (std::getline(stream, line)) {
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
  std::string processed_md = preprocessor(md);
  std::string html;
  int err =
      md_html(processed_md.c_str(), static_cast<MD_SIZE>(processed_md.size()),
              process_output, &html, MD_DIALECT_GITHUB, MD_HTML_FLAG_DEBUG);
  if (err) {
    LOG_ERROR("Can't convert to html the following file: \n");
  }
  return html;
}
