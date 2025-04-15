#pragma once

#include <vector>
#include <utility>
#include <string>
#include <ncurses.h>

bool handleDeleteAction(
    const std::string& currentPath,
    std::vector<std::pair<std::string, bool>>& currentFiles,
    int& selectedIndex,
    int topIndex
);

bool handleRenameAction(
    const std::string& currentPath,
    std::vector<std::pair<std::string, bool>>& currentFiles,
    int& selectedIndex,
    int topIndex
);

