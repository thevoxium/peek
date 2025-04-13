#include "utils.h" 
#include <cstddef>
#include <cstdio>
#include <sys/dirent.h>
#include <vector>
#include <string>
#include <dirent.h> 
#include <cstring>  // For strcmp
#include <iostream>

std::vector<std::string> getDirectoryContents(const std::string &path){
  std::vector<std::string> contents;
  DIR* dir = opendir(path.c_str());

  if(dir==NULL){
    std::string error = "Error opening dir: "+path;
    perror(error.c_str());
    return contents;
  }

  struct dirent* entry;
  while((entry = readdir(dir)) != NULL){
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
      contents.push_back(entry->d_name);
    }
  }
  closedir(dir);
  return contents;
}
