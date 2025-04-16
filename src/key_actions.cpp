#include "key_actions.h"
#include "actions.h"
#include <ncurses.h>

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
      navigateToNextMatch(matchIndices, currentMatchIndex, selectedIndex,
                          topIndex, -1);
    break;
  case 'e':
    exitSearchMode(searchTerm, matchIndices, currentMatchIndex);
    break;
  case 'y':
    handleCopyPathAction(currentPath, currentFiles, selectedIndex);
    break;
  case 'm':
    toggleSortAndRefresh(currentFiles, currentPath, sortByModifiedTime,
                         selectedIndex, topIndex);
    break;
  case '[':
    addBookmark(currentPath);
    refresh();
    break;
  case ']':
    removeBookmark(currentPath);
    refresh();
    break;
  case 'b':
    handleBookmarkListAction(currentPath, currentFiles, selectedIndex,
                             topIndex);
    break;
  case 'o':
    if (!currentFiles.empty() && selectedIndex < (int)currentFiles.size() &&
        !currentFiles[selectedIndex].second) {
      std::string command =
          "open -a \"Brave Browser\" \"" + BUILD_FULL_PATH + "\"";
      int result = system(command.c_str());
    } else {
      refresh();
    }
    break;
  }
}
