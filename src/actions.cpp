#include "actions.h"
#include "utils.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <ncurses.h>
#include <string>

namespace fs = std::filesystem;

std::string toLower(const std::string &s) {
  std::string result = s;
  std::transform(result.begin(), result.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

bool handleDeleteAction(const std::string &currentPath,
                        std::vector<std::pair<std::string, bool>> &currentFiles,
                        int &selectedIndex, int topIndex) {
  if (currentFiles.empty() || selectedIndex >= (int)currentFiles.size()) {
    return false;
  }

  std::string confirmMsg = "Delete?";
  mvprintw(selectedIndex - topIndex + 1, 50, "%s", confirmMsg.c_str());
  refresh();

  int confirm = getch();
  if (confirm == 'y' || confirm == 'Y') {
    try {
      std::string fullPath =
          currentPath == "/"
              ? currentPath + currentFiles[selectedIndex].first
              : currentPath + "/" + currentFiles[selectedIndex].first;

      if (currentFiles[selectedIndex].second) {
        fs::remove_all(fullPath);
      } else {
        fs::remove(fullPath);
      }
      currentFiles = getDirectoryContents(currentPath);
      if (selectedIndex >= (int)currentFiles.size()) {
        selectedIndex = currentFiles.size() - 1;
      }
      return true;
    } catch (const std::exception &e) {
      mvprintw(LINES / 2 + 1, (COLS - 30) / 2,
               "Error: Could not delete file, press any key!");
      refresh();
      getch();
    }
  }
  return false;
}

bool handleRenameAction(const std::string &currentPath,
                        std::vector<std::pair<std::string, bool>> &currentFiles,
                        int &selectedIndex, int topIndex) {
  if (currentFiles.empty() || selectedIndex >= (int)currentFiles.size()) {
    return false;
  }

  std::string confirmMsg = "Rename?";
  mvprintw(selectedIndex - topIndex + 1, 50, "%s", confirmMsg.c_str());
  refresh();

  int confirm = getch();
  if (confirm == 'y' || confirm == 'Y') {
    echo();      // Enable echo to see user input
    curs_set(1); // Show cursor for input
    mvprintw(selectedIndex - topIndex + 2, 50, "New name: ");
    refresh();

    char newName[256];
    getnstr(newName, sizeof(newName) - 1);
    noecho();    // Disable echo again
    curs_set(0); // Hide cursor

    try {
      std::string fullOldPath =
          currentPath == "/"
              ? currentPath + currentFiles[selectedIndex].first
              : currentPath + "/" + currentFiles[selectedIndex].first;
      std::string fullNewPath = currentPath == "/"
                                    ? currentPath + newName
                                    : currentPath + "/" + newName;

      fs::rename(fullOldPath, fullNewPath);
      currentFiles = getDirectoryContents(currentPath);
      if (selectedIndex >= (int)currentFiles.size()) {
        selectedIndex = currentFiles.size() - 1;
      }
      return true;
    } catch (const std::exception &e) {
      mvprintw(LINES / 2 + 1, (COLS - 30) / 2,
               "Error: Could not rename, press any key!");
      refresh();
      getch();
    }
  }
  return false;
}

bool handleEnterDirectoryAction(
    std::string &currentPath,
    std::vector<std::pair<std::string, bool>> &currentFiles, int &selectedIndex,
    int &topIndex) {
  if (currentFiles.empty() || selectedIndex >= (int)currentFiles.size() ||
      !currentFiles[selectedIndex].second) {
    return false;
  }

  try {
    std::string newPath =
        currentPath == "/"
            ? currentPath + currentFiles[selectedIndex].first
            : currentPath + "/" + currentFiles[selectedIndex].first;

    currentPath = newPath;
    currentFiles = getDirectoryContents(currentPath);
    selectedIndex = 0;
    topIndex = 0;
    return true;
  } catch (const std::exception &e) {
    mvprintw(LINES / 2 + 1, (COLS - 30) / 2,
             "Error: Could not enter directory, press any key!");
    refresh();
    getch();
    return false;
  }
}

bool handleGoBackAction(std::string &currentPath,
                        std::vector<std::pair<std::string, bool>> &currentFiles,
                        int &selectedIndex, int &topIndex) {
  if (currentPath == "/") {
    return false; // Already at root
  }

  try {
    size_t lastSlash = currentPath.find_last_of('/');
    if (lastSlash == 0) {
      currentPath = "/"; // Going back to root from first level
    } else {
      currentPath = currentPath.substr(0, lastSlash);
    }

    currentFiles = getDirectoryContents(currentPath);
    selectedIndex = 0;
    topIndex = 0;
    return true;
  } catch (const std::exception &e) {
    mvprintw(LINES / 2 + 1, (COLS - 30) / 2,
             "Error: Could not go back, press any key!");
    refresh();
    getch();
    return false;
  }
}

bool handleSearchAction(std::vector<std::pair<std::string, bool>> &currentFiles,
                        int &selectedIndex, int &topIndex,
                        std::string &searchTerm, std::vector<int> &matchIndices,
                        int &currentMatchIndex) {
  // Clear previous search results
  matchIndices.clear();
  currentMatchIndex = -1;

  // Display search prompt
  move(LINES - 1, 0);
  clrtoeol();
  attron(A_DIM);
  printw("/");
  attroff(A_DIM);

  // Enable input
  echo();
  curs_set(1);

  // Get search term
  char input[256] = {0};
  getnstr(input, sizeof(input) - 1);
  searchTerm = input;

  // Restore terminal state
  noecho();
  curs_set(0);

  if (searchTerm.empty()) {
    return false;
  }

  // Convert search term to lowercase for case-insensitive search
  std::string lowerSearchTerm = toLower(searchTerm);

  // Find all matches
  for (size_t i = 0; i < currentFiles.size(); ++i) {
    std::string lowerFilename = toLower(currentFiles[i].first);
    if (lowerFilename.find(lowerSearchTerm) != std::string::npos) {
      matchIndices.push_back(i);
    }
  }

  // If matches found, navigate to the first one
  if (!matchIndices.empty()) {
    currentMatchIndex = 0;
    selectedIndex = matchIndices[currentMatchIndex];

    // Adjust topIndex if necessary to show the selected item
    if (selectedIndex < topIndex) {
      topIndex = selectedIndex;
    } else if (selectedIndex >= topIndex + LINES - 2) {
      topIndex = selectedIndex - LINES + 3;
      if (topIndex < 0)
        topIndex = 0;
    }

    return true;
  }

  // No matches found
  move(LINES - 1, 0);
  clrtoeol();
  attron(A_DIM);
  printw("Pattern not found: %s", searchTerm.c_str());
  attroff(A_DIM);
  refresh();
  getch(); // Wait for keypress

  return false;
}

void navigateToNextMatch(std::vector<int> &matchIndices, int &currentMatchIndex,
                         int &selectedIndex, int &topIndex, int direction) {
  if (matchIndices.empty()) {
    return;
  }

  // Update current match index (wrap around if needed)
  currentMatchIndex = (currentMatchIndex + direction + matchIndices.size()) %
                      matchIndices.size();

  // Update selected index to point to the match
  selectedIndex = matchIndices[currentMatchIndex];

  // Adjust topIndex if necessary to show the selected item
  if (selectedIndex < topIndex) {
    topIndex = selectedIndex;
  } else if (selectedIndex >= topIndex + LINES - 2) {
    topIndex = selectedIndex - LINES + 3;
    if (topIndex < 0)
      topIndex = 0;
  }
}

void exitSearchMode(std::string &searchTerm, std::vector<int> &matchIndices,
                    int &currentMatchIndex) {
  // Clear search state
  searchTerm.clear();
  matchIndices.clear();
  currentMatchIndex = -1;

  // Clear the status line
  move(LINES - 1, 0);
  clrtoeol();
  refresh();
}
void handleCopyPathAction(
    const std::string &currentPath,
    const std::vector<std::pair<std::string, bool>> &currentFiles,
    int selectedIndex) {
  if (currentFiles.empty() || selectedIndex >= (int)currentFiles.size()) {
    return;
  }

  std::string fullPath =
      currentPath == "/"
          ? currentPath + currentFiles[selectedIndex].first
          : currentPath + "/" + currentFiles[selectedIndex].first;

  std::string command = "printf \"" + fullPath + "\" | pbcopy";
  int result = system(command.c_str());
}

std::string getBookmarksFilePath() {
  std::string homeDir = getenv("HOME") ? getenv("HOME") : ".";
  return homeDir + "/.peek_bookmarks";
}

void addBookmark(const std::string &path) {
  std::string bookmarksFile = getBookmarksFilePath();

  std::vector<std::string> bookmarks = getBookmarks();

  if (std::find(bookmarks.begin(), bookmarks.end(), path) != bookmarks.end()) {
    return; // Already bookmarked
  }

  std::ofstream file(bookmarksFile, std::ios::app);
  if (file.is_open()) {
    file << path << std::endl;
    file.close();
  }
}

bool removeBookmark(const std::string &path) {
  std::string bookmarksFile = getBookmarksFilePath();
  std::vector<std::string> bookmarks = getBookmarks();

  auto it = std::find(bookmarks.begin(), bookmarks.end(), path);
  if (it == bookmarks.end()) {
    return false; // Not found
  }

  bookmarks.erase(it);

  std::ofstream file(bookmarksFile, std::ios::trunc);
  if (file.is_open()) {
    for (const auto &bookmark : bookmarks) {
      file << bookmark << std::endl;
    }
    file.close();
    return true;
  }
  return false;
}

std::vector<std::string> getBookmarks() {
  std::string bookmarksFile = getBookmarksFilePath();
  std::vector<std::string> bookmarks;

  std::ifstream file(bookmarksFile);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      if (!line.empty()) {
        bookmarks.push_back(line);
      }
    }
    file.close();
  }

  return bookmarks;
}

bool handleBookmarkListAction(
    std::string &currentPath,
    std::vector<std::pair<std::string, bool>> &currentFiles, int &selectedIndex,
    int &topIndex) {
  std::vector<std::string> bookmarks = getBookmarks();

  if (bookmarks.empty()) {
    mvprintw(LINES / 2, (COLS - 20) / 2, "No bookmarks found");
    refresh();
    getch();
    return false;
  }

  int bookmarkIndex = 0;
  int bookmarkTopIndex = 0;

  while (true) {
    clear();

    mvprintw(0, 0, "Bookmarks (q to exit, Enter to select, ] to delete):");

    int row = 2;
    for (size_t i = bookmarkTopIndex; i < bookmarks.size() && row < LINES - 1;
         ++i, ++row) {
      if ((int)i == bookmarkIndex) {
        attron(A_REVERSE);
      }

      mvprintw(row, 1, "%s", bookmarks[i].c_str());

      if ((int)i == bookmarkIndex) {
        attroff(A_REVERSE);
      }
    }

    refresh();

    int ch = getch();

    if (ch == 'q') {
      break;
    } else if (ch == KEY_UP || ch == 'k') {
      if (bookmarkIndex > 0) {
        bookmarkIndex--;
        if (bookmarkIndex < bookmarkTopIndex) {
          bookmarkTopIndex = bookmarkIndex;
        }
      }
    } else if (ch == KEY_DOWN || ch == 'j') {
      if (bookmarkIndex < (int)bookmarks.size() - 1) {
        bookmarkIndex++;
        if (bookmarkIndex >= bookmarkTopIndex + LINES - 3) {
          bookmarkTopIndex = bookmarkIndex - LINES + 4;
        }
      }
    } else if (ch == KEY_ENTER || ch == '\n' || ch == '\r') {
      if (!bookmarks.empty() && bookmarkIndex < (int)bookmarks.size()) {
        std::string selectedPath = bookmarks[bookmarkIndex];

        if (fs::exists(selectedPath)) {
          currentPath = selectedPath;
          currentFiles = getDirectoryContents(currentPath);
          selectedIndex = 0;
          topIndex = 0;
          return true;
        } else {
          mvprintw(LINES - 1, 0,
                   "Path no longer exists. Delete bookmark? (y/n)");
          refresh();

          int confirm = getch();
          if (confirm == 'y' || confirm == 'Y') {
            removeBookmark(selectedPath);
            bookmarks = getBookmarks();
            if (bookmarkIndex >= (int)bookmarks.size()) {
              bookmarkIndex = bookmarks.size() - 1;
              if (bookmarkIndex < 0)
                bookmarkIndex = 0;
            }
          }
        }
      }
    } else if (ch == ']') {
      if (!bookmarks.empty() && bookmarkIndex < (int)bookmarks.size()) {
        mvprintw(LINES - 1, 0, "Delete bookmark? (y/n)");
        refresh();

        int confirm = getch();
        if (confirm == 'y' || confirm == 'Y') {
          removeBookmark(bookmarks[bookmarkIndex]);
          bookmarks = getBookmarks();
          if (bookmarkIndex >= (int)bookmarks.size()) {
            bookmarkIndex = bookmarks.size() - 1;
            if (bookmarkIndex < 0)
              bookmarkIndex = 0;
          }
        }
      }
    }
  }

  return false;
}
