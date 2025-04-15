#ifndef PEEK_ICONS_H
#define PEEK_ICONS_H

#include <algorithm>
#include <locale>
#include <ncurses.h> // Include ncurses for COLOR_PAIR
#include <string>
#include <sys/stat.h>
#include <unordered_map>

// --- Define Color Pair Numbers (Start from 1, 0 is default) ---
// Assign numbers sequentially. We'll define the actual colors in main.cpp
#define PAIR_DEFAULT 0   // Represents default terminal color
#define PAIR_DIRECTORY 2 // Blue for directories
#define PAIR_IMAGE 3     // Magenta for images
#define PAIR_VIDEO 4     // Cyan for video
#define PAIR_AUDIO 5     // Yellow for audio
#define PAIR_ARCHIVE 6   // Red for archives
#define PAIR_CODE 7      // Green for code files (C, C++, Py, JS, TS, Shell)
#define PAIR_TEXT 8      // White/Default for text/docs (MD, TXT, PDF, License)
#define PAIR_CONFIG 9    // Yellow for config/JSON/Make
#define PAIR_EXECUTABLE 10 // Bright Green/Cyan for executables
#define PAIR_GIT 11        // Orange/Red for Git

// --- Struct to hold Icon and Color Pair ---
struct IconInfo {
  const char *icon;
  int colorPair; // 0 for default color
};

// --- Icon Definitions (Nerd Fonts) ---
// Now associated with IconInfo struct including color pair
#define ICON_INFO_DIRECTORY {" ", PAIR_DIRECTORY}
#define ICON_INFO_FILE_DEFAULT {"󰈔 ", PAIR_DEFAULT}
#define ICON_INFO_C {" ", PAIR_CODE}
#define ICON_INFO_CPP {" ", PAIR_CODE}
#define ICON_INFO_PYTHON {" ", PAIR_CODE}
#define ICON_INFO_JAVASCRIPT {"󰌞 ", PAIR_CODE}
#define ICON_INFO_TYPESCRIPT {"󰌠 ", PAIR_CODE}
#define ICON_INFO_HTML {"󰌝 ", PAIR_CODE} // Often paired with code
#define ICON_INFO_CSS {"󰌛 ", PAIR_CODE}  // Often paired with code
#define ICON_INFO_JSON {"󰘦 ", PAIR_CONFIG}
#define ICON_INFO_MARKDOWN {"󰍔 ", PAIR_TEXT}
#define ICON_INFO_IMAGE {"󰈟 ", PAIR_IMAGE}
#define ICON_INFO_AUDIO {"󰎈 ", PAIR_AUDIO}
#define ICON_INFO_VIDEO {"󰈫 ", PAIR_VIDEO}
#define ICON_INFO_ARCHIVE {"󰀼 ", PAIR_ARCHIVE}
#define ICON_INFO_PDF {"󰈦 ", PAIR_TEXT}
#define ICON_INFO_TEXT {"󰈙 ", PAIR_TEXT}
#define ICON_INFO_GIT {"󰊢 ", PAIR_GIT}
#define ICON_INFO_MAKEFILE {"󰆍 ", PAIR_CONFIG}
#define ICON_INFO_SHELL {" ", PAIR_CODE}
#define ICON_INFO_LICENSE {"󰿃 ", PAIR_TEXT}
#define ICON_INFO_HEADER {"󰘦 ", PAIR_CODE} // Header often related to code
#define ICON_INFO_OBJECT                                                       \
  {"󰆍 ", PAIR_CONFIG} // Object files often build artifacts
#define ICON_INFO_EXECUTABLE {" ", PAIR_EXECUTABLE}

inline std::string toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

// --- Function now returns IconInfo ---
inline IconInfo getIconForFile(const std::string &filename) {
  // Default icons
  static const IconInfo default_file_icon = ICON_INFO_FILE_DEFAULT;
  static const IconInfo executable_icon = ICON_INFO_EXECUTABLE;
  static const IconInfo directory_icon =
      ICON_INFO_DIRECTORY; // For hidden dir check
  static const IconInfo git_icon = ICON_INFO_GIT;
  static const IconInfo shell_icon = ICON_INFO_SHELL;
  static const IconInfo config_icon =
      ICON_INFO_JSON; // Reuse for generic config

  // --- Maps now store IconInfo ---
  static const std::unordered_map<std::string, IconInfo> exact_match_icons = {
      {"makefile", ICON_INFO_MAKEFILE},
      {"license", ICON_INFO_LICENSE},
      {"readme", ICON_INFO_MARKDOWN},
      {".git", ICON_INFO_GIT}};

  static const std::unordered_map<std::string, IconInfo> extension_icons = {
      {"c", ICON_INFO_C},           {"h", ICON_INFO_HEADER},
      {"hpp", ICON_INFO_HEADER},    {"hxx", ICON_INFO_HEADER},
      {"cpp", ICON_INFO_CPP},       {"cxx", ICON_INFO_CPP},
      {"cc", ICON_INFO_CPP},        {"py", ICON_INFO_PYTHON},
      {"js", ICON_INFO_JAVASCRIPT}, {"jsx", ICON_INFO_JAVASCRIPT},
      {"ts", ICON_INFO_TYPESCRIPT}, {"tsx", ICON_INFO_TYPESCRIPT},
      {"html", ICON_INFO_HTML},     {"htm", ICON_INFO_HTML},
      {"css", ICON_INFO_CSS},       {"sass", ICON_INFO_CSS},
      {"scss", ICON_INFO_CSS},      {"json", ICON_INFO_JSON},
      {"md", ICON_INFO_MARKDOWN},   {"markdown", ICON_INFO_MARKDOWN},
      {"txt", ICON_INFO_TEXT},      {"log", ICON_INFO_TEXT},
      {"cfg", ICON_INFO_JSON},      {"conf", ICON_INFO_JSON},
      {"ini", ICON_INFO_JSON},      {"yaml", ICON_INFO_JSON},
      {"yml", ICON_INFO_JSON},      {"toml", ICON_INFO_JSON},
      {"pdf", ICON_INFO_PDF},       {"png", ICON_INFO_IMAGE},
      {"jpg", ICON_INFO_IMAGE},     {"jpeg", ICON_INFO_IMAGE},
      {"gif", ICON_INFO_IMAGE},     {"bmp", ICON_INFO_IMAGE},
      {"tiff", ICON_INFO_IMAGE},    {"webp", ICON_INFO_IMAGE},
      {"svg", ICON_INFO_IMAGE},     {"ico", ICON_INFO_IMAGE},
      {"mp3", ICON_INFO_AUDIO},     {"wav", ICON_INFO_AUDIO},
      {"ogg", ICON_INFO_AUDIO},     {"flac", ICON_INFO_AUDIO},
      {"aac", ICON_INFO_AUDIO},     {"m4a", ICON_INFO_AUDIO},
      {"opus", ICON_INFO_AUDIO},    {"mp4", ICON_INFO_VIDEO},
      {"mkv", ICON_INFO_VIDEO},     {"mov", ICON_INFO_VIDEO},
      {"avi", ICON_INFO_VIDEO},     {"webm", ICON_INFO_VIDEO},
      {"wmv", ICON_INFO_VIDEO},     {"flv", ICON_INFO_VIDEO},
      {"zip", ICON_INFO_ARCHIVE},   {"rar", ICON_INFO_ARCHIVE},
      {"7z", ICON_INFO_ARCHIVE},    {"tar", ICON_INFO_ARCHIVE},
      {"gz", ICON_INFO_ARCHIVE},    {"bz2", ICON_INFO_ARCHIVE},
      {"xz", ICON_INFO_ARCHIVE},    {"zst", ICON_INFO_ARCHIVE},
      {"deb", ICON_INFO_ARCHIVE},   {"rpm", ICON_INFO_ARCHIVE},
      {"iso", ICON_INFO_ARCHIVE},   {"img", ICON_INFO_ARCHIVE},
      {"sh", ICON_INFO_SHELL},      {"bash", ICON_INFO_SHELL},
      {"zsh", ICON_INFO_SHELL},     {"fish", ICON_INFO_SHELL},
      {"bat", ICON_INFO_SHELL},     {"ps1", ICON_INFO_SHELL},
      {"o", ICON_INFO_OBJECT},      {"so", ICON_INFO_OBJECT},
      {"a", ICON_INFO_OBJECT},      {"lib", ICON_INFO_OBJECT},
      {"dll", ICON_INFO_OBJECT},    {"exe", ICON_INFO_EXECUTABLE}};

  std::string lower_filename = toLower(filename);

  // Check exact filename matches first
  auto exact_it = exact_match_icons.find(lower_filename);
  if (exact_it != exact_match_icons.end()) {
    return exact_it->second;
  }

  // Handle hidden files like .bashrc
  if (filename.length() > 1 && filename[0] == '.' &&
      filename.find('.', 1) == std::string::npos) {
    auto hidden_it = exact_match_icons.find(lower_filename.substr(1));
    if (hidden_it != exact_match_icons.end()) {
      return hidden_it->second;
    }
    if (lower_filename == ".gitignore" || lower_filename == ".gitattributes" ||
        lower_filename == ".gitmodules")
      return git_icon;
    if (lower_filename == ".bashrc" || lower_filename == ".zshrc" ||
        lower_filename == ".profile")
      return shell_icon;
    if (lower_filename == ".config" || lower_filename == ".local" ||
        lower_filename == ".cache")
      return directory_icon; // Should be handled by main logic, but as fallback
    return config_icon;      // Default hidden file to config icon
  }

  // Find the last dot for extension
  size_t dot_pos = filename.rfind('.');
  if (dot_pos != std::string::npos && dot_pos != 0 &&
      dot_pos < filename.length() - 1) {
    std::string extension = toLower(filename.substr(dot_pos + 1));
    auto ext_it = extension_icons.find(extension);
    if (ext_it != extension_icons.end()) {
      return ext_it->second;
    }
  }

  // Check if executable
  struct stat st;
  if (stat(filename.c_str(), &st) == 0) {
    if (st.st_mode & S_IXUSR || st.st_mode & S_IXGRP || st.st_mode & S_IXOTH) {
      // Check if it's a known script type first
      if (dot_pos != std::string::npos && dot_pos != 0 &&
          dot_pos < filename.length() - 1) {
        std::string extension = toLower(filename.substr(dot_pos + 1));
        if (extension == "sh" || extension == "bash" || extension == "zsh" ||
            extension == "fish" || extension == "py" || extension == "rb" ||
            extension == "pl") {
          // Let extension map handle it (or fall through if not mapped)
        } else {
          return executable_icon; // Executable but not a mapped script
        }
      } else {
        return executable_icon; // Executable without common script extension
      }
    }
  }

  // Fallback to default file icon if no other match
  return default_file_icon;
}

#endif // PEEK_ICONS_H
