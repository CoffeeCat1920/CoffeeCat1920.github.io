#include <filesystem>

namespace fs = std::filesystem;

namespace generator {
void Generate(const fs::path &content_dir, const fs::path &dist_dir);
}
