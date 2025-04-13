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
          return 1; // Exit if CWD cannot be determined
      }
  } else if (argc == 2) {
      initialPath = argv[1];
      if (!isValidPath(initialPath)) {
          fprintf(stderr, "peek: Invalid path: %s\n", initialPath.c_str());
          return 1; // Exit if path is invalid
      }
  } else {
      fprintf(stderr, "Usage: %s [<directory_path>]\n", argv[0]);
      return 1;
  }

  setlocale(LC_ALL, "");
  initscr();             
  noecho();              
  cbreak();              
  keypad(stdscr, TRUE);  
  curs_set(0);           


  int selectedIndex = 0;
  int topIndex = 0;
  std::vector<std::pair<std::string, bool>> currentFiles;
  std::string currentPath = initialPath;

  int ch;
  while(true){

    currentFiles = getDirectoryContents(currentPath);

    if (selectedIndex < 0) selectedIndex = 0;
    if (!currentFiles.empty() && selectedIndex >= currentFiles.size()) selectedIndex = currentFiles.size()-1;
    if (currentFiles.empty()) selectedIndex = 0;
    clear();

    mvprintw(0, 1, "%s", ("CWD: "+currentPath).c_str());

    int row = 1; 
    for (size_t i = topIndex; i < currentFiles.size() && row < LINES - 1; ++i, ++row) {
        if ((int)i == selectedIndex) {
            attron(A_REVERSE);
        }

        if(currentFiles[i].second == true){
          mvprintw(row, 1, "%s", ("🗂️"+currentFiles[i].first.substr(0, SUBSTR_LEN)).c_str());
        }else{
          mvprintw(row, 1, "%s", (currentFiles[i].first.substr(0, SUBSTR_LEN)).c_str());
        }
        if ((int)i == selectedIndex) {
            attroff(A_REVERSE);
        }
    }

    refresh();

    ch = getch();
    if (ch == 'q') {
        break; // Exit the loop
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
                topIndex = selectedIndex - LINES + 2;
            }
        }
    }

  }

  endwin();
  return 0;
}
