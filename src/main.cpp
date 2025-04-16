#include "icons.h"
#include "key_actions.h"
#include "utils.h"
#include <algorithm>
#include <cstdio>
#include <limits.h>
#include <ncurses.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include <vector>

#define SUBSTR_LEN COLS / 2

int main(int argc, char *argv[]) {

  std::string initialPath;
  if (parseInitialPath(argc, argv, initialPath) != 0) {
    return 1;
  }

  NCURSES_SET_BASICS;
  NCURSES_COLOR_SET;

  // Navigation variables
  int selectedIndex = 0;
  int topIndex = 0;

  // File management variables
  bool inDeleteMode = false;
  std::string currentPath = initialPath;
  std::vector<std::pair<std::string, bool>> currentFiles;
  currentFiles = getDirectoryContents(currentPath);

  // Search-related variables
  std::string searchTerm;
  std::vector<int> matchIndices;
  int currentMatchIndex = -1;

  // Display and sorting variables
  bool sortByModifiedTime = false; // Flag to track sort state
  std::string lastKeyPressed;
  int keyDisplayTimeout = 0;
  int ch;

  while (true) {

    if (selectedIndex < 0 || currentFiles.empty())
      selectedIndex = 0;
    if (!currentFiles.empty() && selectedIndex >= currentFiles.size())
      selectedIndex = currentFiles.size() - 1;

    clear();

    int row = 1;
    for (size_t i = topIndex; i < currentFiles.size() && row < LINES - 1;
         ++i, ++row) {

      std::string displayName = currentFiles[i].first;
      IconInfo iconInfo;
      bool isSelected = ((int)i == selectedIndex);
      int selectionAttrs = 0;

      bool isSearchMatch = false;
      // Check if this item is a search match
      if (!searchTerm.empty()) {
        std::string lowerFilename = toLower(displayName);
        std::string lowerSearchTerm = toLower(searchTerm);
        isSearchMatch =
            (lowerFilename.find(lowerSearchTerm) != std::string::npos);
      }

      if (!lastKeyPressed.empty()) {
        move(LINES - 1, COLS - lastKeyPressed.length() - 1);
        attron(A_DIM);
        printw("%s", lastKeyPressed.c_str());
        attroff(A_DIM);

        if (--keyDisplayTimeout <= 0) {
          lastKeyPressed.clear();
        }
      }

      if (currentFiles[i].second == true) {
        iconInfo = ICON_INFO_DIRECTORY;
        if (displayName == ".git") {
          iconInfo = ICON_INFO_GIT;
        }
      } else {
        iconInfo = getIconForFile(displayName);
      }

      // Determine selection attributes *before* printing anything on the line
      if (isSelected) {
        if (inDeleteMode) {
          selectionAttrs = COLOR_PAIR(1) | A_REVERSE;
        } else {
          selectionAttrs = A_REVERSE | A_DIM;
        }
        attron(selectionAttrs); // Apply selection highlight for the whole line
      } else if (isSearchMatch) {
        // Highlight search matches with bold
        attron(A_BOLD);
      }

      // Print Icon: Apply specific color only if NOT selected
      move(row, 1); // Position cursor for icon
      bool iconColorApplied = false;
      if (!isSelected && iconInfo.colorPair != PAIR_DEFAULT && has_colors()) {
        attron(COLOR_PAIR(iconInfo.colorPair));
        iconColorApplied = true;
      }
      printw("%s", iconInfo.icon); // Print the icon (inherits selectionAttrs if
                                   // isSelected)
      if (iconColorApplied) {
        attroff(COLOR_PAIR(
            iconInfo.colorPair)); // Turn off specific icon color immediately
      }

      // Print Filename: Inherits selectionAttrs if isSelected, otherwise
      // default color
      printw("%s", displayName.substr(0, SUBSTR_LEN).c_str());

      if (!currentFiles[i].second) {
        std::string fullPath = currentPath == "/"
                                   ? currentPath + displayName
                                   : currentPath + "/" + displayName;
        std::string modTime = getFormattedModTime(fullPath);
        if (!modTime.empty()) {

          move(row, COLS - modTime.length() - 2); // Right-align with padding
          attron(COLOR_PAIR(PAIR_DIRECTORY) | A_DIM);
          printw("%s", modTime.c_str());
          attroff(COLOR_PAIR(PAIR_DIRECTORY) | A_DIM);
        }
      }

      // Fill rest of selected line & turn off selection attributes
      if (isSelected) {
        int iconVisualWidth = 2;
        int current_col =
            1 + iconVisualWidth + displayName.substr(0, SUBSTR_LEN).length();
        // The selectionAttrs are already on, just fill
        while (current_col <
               std::min(static_cast<int>(displayName.length()), SUBSTR_LEN)) {
          mvaddch(row, current_col++, ' ');
        }
        attroff(selectionAttrs); // Turn off selection highlight after filling
      } else if (isSearchMatch) {
        attroff(A_BOLD); // Turn off bold for search matches
      }
    }

    attroff(A_REVERSE | A_DIM | A_BOLD);

    // Display search status if in search mode
    if (!searchTerm.empty() && !matchIndices.empty()) {
      move(LINES - 1, 0);
      clrtoeol();
      attron(A_DIM);
      printw("/%s (%d/%d)", searchTerm.c_str(), currentMatchIndex + 1,
             (int)matchIndices.size());
      attroff(A_DIM);
    }

    refresh();

    ch = getch();

    if (ch != ERR) {
      lastKeyPressed = keyname(ch);
      keyDisplayTimeout = 10; // Show for 10 refresh cycles
    }

    handleKeyPress(ch, currentPath, currentFiles, selectedIndex, topIndex,
                   inDeleteMode, sortByModifiedTime, searchTerm, matchIndices,
                   currentMatchIndex);
  }

  endwin();
  return 0;
}
