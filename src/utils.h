#ifndef UTILS_H
#define UTILS_H

#include <ctime>
#include <string>
#include <utility>
#include <vector>

std::vector<std::pair<std::string, bool>>
getDirectoryContents(const std::string &path);

std::string getFormattedModTime(const std::string &path);
#endif // UTILS_H
