#include "utils.h" 
#include <cstddef>
#include <cstdio>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <utility>
#include <vector>
#include <string>
#include <dirent.h> 
#include <cstring>  // For strcmp
#include <iostream>

std::vector<std::pair<std::string, bool>> getDirectoryContents(const std::string &path){
  std::vector<std::pair<std::string, bool>> contents;
  DIR* dir = opendir(path.c_str());

  if(dir==NULL){
    std::string error = "Error opening dir: "+path;
    perror(error.c_str());
    return contents;
  }

  struct dirent* entry;
  while((entry = readdir(dir)) != NULL){
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
      std::string fullPath = path+"/"+entry->d_name;
      struct stat buffer;
      if(stat(fullPath.c_str(), &buffer) == 0){
        contents.push_back({entry->d_name, S_ISDIR(buffer.st_mode)});
      }
    }

  }
  closedir(dir);
  return contents;
}
