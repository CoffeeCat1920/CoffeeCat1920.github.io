#include "common.hpp"

RawDocument::RawDocument(fs::path &contentDir) : contentDir(contentDir) {
  destDir = toDistPath(contentDir, "html");
  contentDir = common::fileToString(contentDir);
}

fs::path RawDocument::toDistPath(const fs::path &file, const std::string &ext) {
  fs::path out = "dist" / fs::relative(file, "content");
  if (ext != "")
    out.replace_extension(ext);
  return out;
}

ParsedDocument::ParsedDocument(std::string &content) : content(content) {}

ParsedDocument common::parseDocument(RawDocument &rawDoc) {
  ParsedDocument parsedDoc(rawDoc.content);
}
