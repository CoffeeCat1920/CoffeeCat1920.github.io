#include "generator.hpp"
#include "logger/logger.hpp"
#include "md/md.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

std::map<std::string, std::string> template_map{};

fs::path toDistPath(const fs::path &file, const std::string &ext = "") {
  fs::path out = "dist" / fs::relative(file, "content");
  if (ext != "")
    out.replace_extension(ext);
  return out;
}

std::string open_file(const fs::path &file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    LOG_ERROR("Can't Open file {}", file_path.string());
    return "";
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void generate_md(const fs::path &file_path) {
  std::string md_content = open_file(file_path);
  LOG("Generating html for: {}", file_path.string());
  std::string html_content = md_to_html::Convert(md_content);
  fs::path html_path = toDistPath(file_path, "html");
  fs::create_directories(html_path.parent_path());
  std::ofstream html_file(html_path);
  LOG("Saving html at: {}", html_path.string());
  html_file << html_content;
}

void generator::Generate(const fs::path &content_dir,
                         const fs::path &dist_dir) {

  if (content_dir.filename() == "_template") {
    LOG("Found the _template dir");
    for (const auto &entry : fs::directory_iterator(content_dir)) {
      std::string ext = entry.path().extension();
      if (ext == ".md") {
        generate_md(entry.path());
        std::string html_path = toDistPath(entry.path(), "html");
        std::string html_file = open_file(html_path);
        template_map[entry.path().filename()] = html_file;
      }
    }
    return;
  }

  LOG("Generating site from: {}", content_dir.string());

  if (!fs::is_directory(content_dir)) {
    LOG_ERROR("Not a directory {}", content_dir.string());
    return;
  }

  for (const auto &entry : fs::directory_iterator(content_dir)) {
    if (entry.is_directory()) {
      Generate(entry.path(), dist_dir);
    }
    if (entry.is_regular_file()) {
      std::string ext = entry.path().extension();
      if (ext == ".md")
        generate_md(entry.path());
    }
  }
}
