
#pragma once
#include <string>
#include <vector>

void handleMoveUp(int &selectedIndex, int &topIndex);
void handleMoveDown(
    const std::vector<std::pair<std::string, bool>> &currentFiles,
    int &selectedIndex, int &topIndex);
void handleKeyPress(int ch, std::string &currentPath,
                    std::vector<std::pair<std::string, bool>> &currentFiles,
                    int &selectedIndex, int &topIndex, bool &inDeleteMode,
                    bool &sortByModifiedTime, std::string &searchTerm,
                    std::vector<int> &matchIndices, int &currentMatchIndex);
