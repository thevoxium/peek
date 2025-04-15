#include "utils.h"
#include <cstddef>
#include <cstdio>
#include <cstring> // For strcmp
#include <ctime>
#include <dirent.h>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

std::vector<std::pair<std::string, bool>>
getDirectoryContents(const std::string &path) {
  std::vector<std::pair<std::string, bool>> contents;
  DIR *dir = opendir(path.c_str());

  if (dir == NULL) {
    std::string error = "Error opening dir: " + path;
    perror(error.c_str());
    return contents;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
      std::string fullPath = path + "/" + entry->d_name;
      struct stat buffer;
      if (stat(fullPath.c_str(), &buffer) == 0) {
        contents.push_back({entry->d_name, S_ISDIR(buffer.st_mode)});
      }
    }
  }
  closedir(dir);
  return contents;
}

std::string getFormattedModTime(const std::string &path) {
  try {
    auto ftime = fs::last_write_time(path);
    auto sctp =
        std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() +
            std::chrono::system_clock::now());
    std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
    std::tm tm = *std::localtime(&cftime);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%b %d %H:%M");
    return oss.str();
  } catch (...) {
    return "";
  }
}
