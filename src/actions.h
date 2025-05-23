#pragma once

#include <ncurses.h>
#include <string>
#include <utility>
#include <vector>

#define BUILD_FULL_PATH                                                        \
  ((currentPath == "/")                                                        \
       ? (currentPath + currentFiles[selectedIndex].first)                     \
       : (currentPath + "/" + currentFiles[selectedIndex].first))

bool handleDeleteAction(const std::string &currentPath,
                        std::vector<std::pair<std::string, bool>> &currentFiles,
                        int &selectedIndex, int topIndex);

bool handleRenameAction(const std::string &currentPath,
                        std::vector<std::pair<std::string, bool>> &currentFiles,
                        int &selectedIndex, int topIndex);

bool handleEnterDirectoryAction(
    std::string &currentPath,
    std::vector<std::pair<std::string, bool>> &currentFiles, int &selectedIndex,
    int &topIndex);

bool handleGoBackAction(std::string &currentPath,
                        std::vector<std::pair<std::string, bool>> &currentFiles,
                        int &selectedIndex, int &topIndex);

bool handleSearchAction(std::vector<std::pair<std::string, bool>> &currentFiles,
                        int &selectedIndex, int &topIndex,
                        std::string &searchTerm, std::vector<int> &matchIndices,
                        int &currentMatchIndex);

void navigateToNextMatch(std::vector<int> &matchIndices, int &currentMatchIndex,
                         int &selectedIndex, int &topIndex, int direction);

void exitSearchMode(std::string &searchTerm, std::vector<int> &matchIndices,
                    int &currentMatchIndex);

void handleCopyPathAction(
    const std::string &currentPath,
    const std::vector<std::pair<std::string, bool>> &currentFiles,
    int selectedIndex);

void addBookmark(const std::string &path);
bool removeBookmark(const std::string &path);
std::vector<std::string> getBookmarks();
bool handleBookmarkListAction(
    std::string &currentPath,
    std::vector<std::pair<std::string, bool>> &currentFiles, int &selectedIndex,
    int &topIndex);
