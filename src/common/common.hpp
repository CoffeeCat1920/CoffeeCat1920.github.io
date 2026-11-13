#include <filesystem>
#include <map>
#include <string>

namespace fs = std::filesystem;

struct ParsedDocument {
  std::map<std::string, std::string> vars{};
  std::string &content;

  ParsedDocument(std::string &content);
};

struct RawDocument {
  fs::path contentDir;
  fs::path destDir;
  std::map<std::string, std::string> templates{};
  std::string content;

  RawDocument(fs::path &contentDir);

private:
  fs::path toDistPath(const fs::path &file, const std::string &ext = "");
};

namespace common {
std::string fileToString(fs::path &filePath);
ParsedDocument parseDocument(RawDocument &rawdoc);
} // namespace common
