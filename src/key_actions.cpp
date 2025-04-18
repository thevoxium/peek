#include "key_actions.h"
#include "actions.h"
#include <fstream>
#include <ncurses.h>
#include <unordered_map>

// Load application list from config file.
static std::unordered_map<std::string, std::string> loadAppList() {
  std::unordered_map<std::string, std::string> m;
  std::ifstream file("../app_list.txt");
  if (!file.is_open())
    return m;
  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    auto pos = line.find('=');
    if (pos == std::string::npos)
      continue;
    std::string ext = line.substr(0, pos);
    std::string app = line.substr(pos + 1);
    m[ext] = app;
  }
  return m;
}

// Singleton accessor for the extension→app map.
static const std::unordered_map<std::string, std::string> &getAppMap() {
  static const auto map = loadAppList();
  return map;
}

#define BUILD_FULL_PATH                                                        \
  ((currentPath == "/")                                                        \
       ? (currentPath + currentFiles[selectedIndex].first)                     \
       : (currentPath + "/" + currentFiles[selectedIndex].first))

void handleMoveUp(int &selectedIndex, int &topIndex) {
  if (selectedIndex > 0) {
    selectedIndex--;
    if (selectedIndex < topIndex)
      topIndex = selectedIndex;
  }
}

void handleMoveDown(
    const std::vector<std::pair<std::string, bool>> &currentFiles,
    int &selectedIndex, int &topIndex) {
  if (!currentFiles.empty() && selectedIndex < (int)currentFiles.size() - 1) {
    selectedIndex++;
    if (selectedIndex >= topIndex + LINES - 2) {
      topIndex = selectedIndex - LINES + 3;
      if (topIndex < 0)
        topIndex = 0;
    }
  }
}

// Big switch-case or command map
void handleKeyPress(int ch, std::string &currentPath,
                    std::vector<std::pair<std::string, bool>> &currentFiles,
                    int &selectedIndex, int &topIndex, bool &inDeleteMode,
                    bool &sortByModifiedTime, std::string &searchTerm,
                    std::vector<int> &matchIndices, int &currentMatchIndex) {
  switch (ch) {
  case 'q':
    endwin();
    exit(0);
  case KEY_UP:
  case 'k':
    handleMoveUp(selectedIndex, topIndex);
    break;
  case KEY_DOWN:
  case 'j':
    handleMoveDown(currentFiles, selectedIndex, topIndex);
    break;
  case 'd':
    inDeleteMode = true;
    if (handleDeleteAction(currentPath, currentFiles, selectedIndex,
                           topIndex)) {
      inDeleteMode = false;
    }
    break;
  case 'r':
    handleRenameAction(currentPath, currentFiles, selectedIndex, topIndex);
    break;
  case 'l':
  case KEY_ENTER:
  case '\n':
  case '\r':
  case KEY_RIGHT:
    handleEnterDirectoryAction(currentPath, currentFiles, selectedIndex,
                               topIndex);
    break;
  case 'h':
  case KEY_LEFT:
    handleGoBackAction(currentPath, currentFiles, selectedIndex, topIndex);
    break;
  case '/':
    handleSearchAction(currentFiles, selectedIndex, topIndex, searchTerm,
                       matchIndices, currentMatchIndex);
    break;
  case 'n':
    if (!matchIndices.empty())
      navigateToNextMatch(matchIndices, currentMatchIndex, selectedIndex,
                          topIndex, 1);
    break;
  case 'N':
    if (!matchIndices.empty())
    case 'o':
      if (!currentFiles.empty() && selectedIndex < (int)currentFiles.size() &&
          !currentFiles[selectedIndex].second) {
        std::string fullPath = BUILD_FULL_PATH;
        auto dotPos = fullPath.find_last_of('.');
        std::string ext;
        if (dotPos != std::string::npos)
          ext = fullPath.substr(dotPos + 1);
        const auto &appMap = getAppMap();
        // Default to Brave Browser if no mapping found
        std::string app = "Brave Browser";
        auto it = appMap.find(ext);
        if (it != appMap.end())
          app = it->second;
        std::string command = "open -a \"" + app + "\" \"" + fullPath + "\"";
        system(command.c_str());
      } else {
        refresh();
      }
    break;
    removeBookmark(currentPath);
    refresh();
    break;
  case 'b':
    handleBookmarkListAction(currentPath, currentFiles, selectedIndex,
                             topIndex);
    break;
  }
}
