#include "md4c-html.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static void process_output(const MD_CHAR *text, MD_SIZE size, void *userdata) {
  auto *out = static_cast<std::string *>(userdata);
  out->append(text, size);
}

int main() {

  std::string md_file_path = "index.md";
  std::string html_file_path = "index.html";

  std::ifstream md_file(md_file_path);
  if (!md_file.is_open()) {
    std::cerr << "Failed to open file " << md_file_path << std::endl;
    return 1;
  }
  std::stringstream buffer;
  buffer << md_file.rdbuf();

  std::string markdown = buffer.str();
  std::string html;
  int err =
      md_html(markdown.c_str(), static_cast<MD_SIZE>(markdown.size()),
              process_output, &html, MD_DIALECT_GITHUB, MD_HTML_FLAG_DEBUG);

  if (err != 0) {
    std::cerr << "Failed to parse markdown." << std::endl;
    return 1;
  }

  std::ofstream html_file(html_file_path);
  if (!html_file.is_open()) {
    std::cerr << "Failed to open html." << html_file_path << std::endl;
    return 1;
  }
  html_file << html;

  std::cout << "Original Markdown:\n" << markdown << "\n\n";
  std::cout << "Rendered HTML:\n" << html << std::endl;

  return 0;
}
