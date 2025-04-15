#include <exception>
#include <ncurses.h>
#include <utility>
#include <vector>
#include <string>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <cstdio>
#include <locale.h>
#include "utils.h"
#include "actions.h"

#define SUBSTR_LEN 40


bool isValidPath(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

int main(int argc, char* argv[]){
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
      init_pair(1, COLOR_RED, -1);
  }

  keypad(stdscr, TRUE);
  curs_set(0);


  int selectedIndex = 0;
  int topIndex = 0;
  bool inDeleteMode = false;
  std::vector<std::pair<std::string, bool>> currentFiles;
  std::string currentPath = initialPath;
currentFiles = getDirectoryContents(currentPath);
  int ch;
  while(true){


    if (selectedIndex < 0) selectedIndex = 0;
    if (!currentFiles.empty() && selectedIndex >= currentFiles.size()) selectedIndex = currentFiles.size()-1;
    if (currentFiles.empty()) selectedIndex = 0;
    clear();

    mvprintw(0, 1, "%s", ("CWD: "+currentPath).c_str());



    int row = 1;
    for (size_t i = topIndex; i < currentFiles.size() && row < LINES - 1; ++i, ++row) {

     if ((int)i == selectedIndex) {
          if (inDeleteMode) {
              attron(COLOR_PAIR(1) | A_REVERSE);
          } else {
              attron(A_REVERSE | A_DIM);
          }
      }
        if(currentFiles[i].second == true){
          mvprintw(row, 1, "%s", ("🗂️"+currentFiles[i].first.substr(0, SUBSTR_LEN)).c_str());
        }else{
          mvprintw(row, 1, "%s", ("📄"+currentFiles[i].first.substr(0, SUBSTR_LEN)).c_str());
        }
        if ((int)i == selectedIndex) {
             int current_col = 1 + 2 + currentFiles[i].first.substr(0, SUBSTR_LEN).length();
             while(current_col < COLS) {
                 mvaddch(row, current_col++, ' ');
             }
            attroff(A_REVERSE);
        }
    }


    int term_height = LINES;
    int term_width = COLS;
    int statusBarRow = term_height - 1;
    std::string fullPathStatus;

    if (!currentFiles.empty() && selectedIndex >= 0 && selectedIndex < (int)currentFiles.size()) {
        if (currentPath == "/") {
            fullPathStatus = currentPath + currentFiles[selectedIndex].first;
        } else {
            fullPathStatus = currentPath + "/" + currentFiles[selectedIndex].first;
        }
    } else {
        fullPathStatus = currentPath;
        if (!fullPathStatus.empty() && fullPathStatus.back() != '/') {
            fullPathStatus += "/";
        }
         if (currentFiles.empty()) {
             fullPathStatus += " (empty)";
         }
    }

    attron(A_REVERSE | A_DIM);
    move(statusBarRow, 0);
    clrtoeol();
    mvprintw(statusBarRow, 0, "%.*s", term_width, fullPathStatus.c_str());
    int printedLen = fullPathStatus.length() > (size_t)term_width ? term_width : fullPathStatus.length();
    for (int k = printedLen; k < term_width; ++k) {
        mvaddch(statusBarRow, k, ' ');
    }
    attroff(A_REVERSE | A_DIM);


    refresh();

    ch = getch();
    if (ch == 'q') {
        break;
    } else if (ch == KEY_UP) {
        if (selectedIndex > 0) {
            selectedIndex--;
            if (selectedIndex < topIndex) {
                topIndex = selectedIndex;
            }
        }
    } else if (ch == KEY_DOWN) {
        if (!currentFiles.empty() && selectedIndex < (int)currentFiles.size() - 1) {
            selectedIndex++;
            if (selectedIndex >= topIndex + LINES - 2) {
                 topIndex = selectedIndex - LINES + 3; // Adjusted slightly for status bar
                 if (topIndex < 0) topIndex = 0;
            }
        }
    } else if (ch == 'd') {
        inDeleteMode = true;
        if (handleDeleteAction(currentPath, currentFiles, selectedIndex, topIndex)) {
            inDeleteMode = false;
        }
    } else if (ch == 'r') {
        handleRenameAction(currentPath, currentFiles, selectedIndex, topIndex);
    }
  }

  endwin();
  return 0;
}
