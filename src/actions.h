#pragma once

#include <ncurses.h>
#include <string>
#include <utility>
#include <vector>

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
