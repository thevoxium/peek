#include "actions.h"
#include "icons.h"
#include "utils.h"
#include <cstdio>
#include <limits.h>
#include <locale.h>
#include <ncurses.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <utility>
#include <vector>

#define SUBSTR_LEN 100

bool isValidPath(const std::string &path) {
  struct stat buffer;
  return (stat(path.c_str(), &buffer) == 0);
}

int main(int argc, char *argv[]) {
  std::string initialPath;
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

  setlocale(LC_ALL, "");
  initscr();
  noecho();
  cbreak();
  if (has_colors()) {
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1); // Reserved for delete mode highlight
    // Initialize color pairs based on defines in icons.h
    init_pair(PAIR_DIRECTORY, COLOR_BLUE, -1);
    init_pair(PAIR_IMAGE, COLOR_MAGENTA, -1);
    init_pair(PAIR_VIDEO, COLOR_CYAN, -1);
    init_pair(PAIR_AUDIO, COLOR_YELLOW, -1);
    init_pair(PAIR_ARCHIVE, COLOR_RED, -1);
    init_pair(PAIR_CODE, COLOR_GREEN, -1);
    init_pair(PAIR_TEXT, COLOR_WHITE, -1);
    init_pair(PAIR_CONFIG, COLOR_YELLOW, -1);
    init_pair(PAIR_EXECUTABLE, COLOR_GREEN, -1);
    init_pair(PAIR_GIT, COLOR_RED, -1);
    // Add more init_pair calls if more PAIR_XXX constants exist
  }
  keypad(stdscr, TRUE);
  curs_set(0);

  int selectedIndex = 0;
  int topIndex = 0;
  bool inDeleteMode = false;
  std::vector<std::pair<std::string, bool>> currentFiles;
  std::string currentPath = initialPath;
  currentFiles = getDirectoryContents(currentPath);

  // Search-related variables
  std::string searchTerm;
  std::vector<int> matchIndices;
  int currentMatchIndex = -1;

  int ch;
  while (true) {

    if (selectedIndex < 0)
      selectedIndex = 0;
    if (!currentFiles.empty() && selectedIndex >= currentFiles.size())
      selectedIndex = currentFiles.size() - 1;
    if (currentFiles.empty())
      selectedIndex = 0;
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

      // Fill rest of selected line & turn off selection attributes
      if (isSelected) {
        int iconVisualWidth = 2;
        int current_col =
            1 + iconVisualWidth + displayName.substr(0, SUBSTR_LEN).length();
        // The selectionAttrs are already on, just fill
        while (current_col < COLS) {
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
    if (ch == 'q') {
      break;
    } else if (ch == KEY_UP || ch == 'k') {
      if (selectedIndex > 0) {
        selectedIndex--;
        if (selectedIndex < topIndex) {
          topIndex = selectedIndex;
        }
      }
    } else if (ch == KEY_DOWN || ch == 'j') {
      if (!currentFiles.empty() &&
          selectedIndex < (int)currentFiles.size() - 1) {
        selectedIndex++;
        if (selectedIndex >= topIndex + LINES - 2) {
          topIndex =
              selectedIndex - LINES + 3; // Adjusted slightly for status bar
          if (topIndex < 0)
            topIndex = 0;
        }
      }
    } else if (ch == 'd') {
      inDeleteMode = true;
      if (handleDeleteAction(currentPath, currentFiles, selectedIndex,
                             topIndex)) {
        inDeleteMode = false;
      }
    } else if (ch == 'r') {
      handleRenameAction(currentPath, currentFiles, selectedIndex, topIndex);
    } else if (ch == 'l' || ch == KEY_ENTER || ch == '\n' || ch == '\r') {
      handleEnterDirectoryAction(currentPath, currentFiles, selectedIndex,
                                 topIndex);
    } else if (ch == 'h') {
      handleGoBackAction(currentPath, currentFiles, selectedIndex, topIndex);
    } else if (ch == '/') {
      // Enter search mode
      handleSearchAction(currentFiles, selectedIndex, topIndex, searchTerm,
                         matchIndices, currentMatchIndex);
    } else if (ch == 'n' && !matchIndices.empty()) {
      // Navigate to next match
      navigateToNextMatch(matchIndices, currentMatchIndex, selectedIndex,
                          topIndex, 1);
    } else if (ch == 'N' && !matchIndices.empty()) {
      // Navigate to previous match
      navigateToNextMatch(matchIndices, currentMatchIndex, selectedIndex,
                          topIndex, -1);
    } else if (ch == 'e') { // Escape key
      // Exit search mode
      exitSearchMode(searchTerm, matchIndices, currentMatchIndex);
    }
  }

  endwin();
  return 0;
}
