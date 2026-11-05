#include "md.hpp"
#include "logger/logger.hpp"
#include <md4c-html.h>
#include <md4c.h>

static void process_output(const MD_CHAR *text, MD_SIZE size, void *userdata) {
  auto *out = static_cast<std::string *>(userdata);
  out->append(text, size);
}

std::string md_to_html::Convert(std::string md) {
  std::string html;
  int err = md_html(md.c_str(), static_cast<MD_SIZE>(md.size()), process_output,
                    &html, MD_DIALECT_GITHUB, MD_HTML_FLAG_DEBUG);
  if (err) {
    LOG_ERROR("Can't convert to html the following file: \n");
  }
  return html;
}
