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

bool isValidPath(const std::string &path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}
int parseInitialPath(int argc, char *argv[], std::string &initialPath) {
  if (argc == 1) {
    char cwd_buffer[PATH_MAX];
    if (getcwd(cwd_buffer, sizeof(cwd_buffer)) != NULL) {
      initialPath = cwd_buffer;
    } else {
      perror("peek: Error getting current working directory");
      return 1;
    }
  } else if (argc == 2) {
    initialPath = argv[1];
    if (!isValidPath(initialPath)) {
      fprintf(stderr, "peek: Invalid path: %s\n", initialPath.c_str());
      return 1;
    }
  } else {
    fprintf(stderr, "Usage: %s [<directory_path>]\n", argv[0]);
    return 1;
  }
  return 0;
}
