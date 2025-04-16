#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <limits.h>
#include <locale.h>
#include <ncurses.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include <vector>

#define BUILD_FULL_PATH                                                        \
  ((currentPath == "/")                                                        \
       ? (currentPath + currentFiles[selectedIndex].first)                     \
       : (currentPath + "/" + currentFiles[selectedIndex].first))

#define NCURSES_COLOR_SET                                                      \
  if (has_colors()) {                                                          \
    start_color();                                                             \
    use_default_colors();                                                      \
    init_pair(1, COLOR_RED, -1);                                               \
    init_pair(PAIR_DIRECTORY, COLOR_BLUE, -1);                                 \
    init_pair(PAIR_IMAGE, COLOR_MAGENTA, -1);                                  \
    init_pair(PAIR_VIDEO, COLOR_CYAN, -1);                                     \
    init_pair(PAIR_AUDIO, COLOR_YELLOW, -1);                                   \
    init_pair(PAIR_ARCHIVE, COLOR_RED, -1);                                    \
    init_pair(PAIR_CODE, COLOR_GREEN, -1);                                     \
    init_pair(PAIR_TEXT, COLOR_WHITE, -1);                                     \
    init_pair(PAIR_CONFIG, COLOR_YELLOW, -1);                                  \
    init_pair(PAIR_EXECUTABLE, COLOR_GREEN, -1);                               \
    init_pair(PAIR_GIT, COLOR_RED, -1);                                        \
  }

#define NCURSES_SET_BASICS                                                     \
  setlocale(LC_ALL, "");                                                       \
  initscr();                                                                   \
  noecho();                                                                    \
  cbreak();                                                                    \
  keypad(stdscr, TRUE);                                                        \
  curs_set(0);

bool isValidPath(const std::string &path);

int parseInitialPath(int argc, char *argv[], std::string &initialPath);

std::vector<std::pair<std::string, bool>>
getDirectoryContents(const std::string &path);

std::string getFormattedModTime(const std::string &path);
#endif // UTILS_H
