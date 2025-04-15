#include "actions.h"
#include "utils.h"
#include <filesystem>
#include <ncurses.h>
#include <string>

namespace fs = std::filesystem;

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
