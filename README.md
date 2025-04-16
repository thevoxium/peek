
# peek

![peek demo](https://media.githubusercontent.com/media/thevoxium/peek/refs/heads/main/assets/img.gif)

A lightweight, fast terminal file browser.

> Because MacOS finder sucks

## Features

- File and directory operations (rename, delete)
- Search functionality with match highlighting
- Bookmarking system for quick access to favorite directories
- File type icons with color coding
- Path copying to clipboard
- Sort by modified time

## Installation

### Prerequisites

```bash
# Install ncurses library
brew install ncurses

# Install Nerd Font for icons (recommended)
brew tap homebrew/cask-fonts
brew install --cask font-hack-nerd-font
```

### Building from source

```bash
# Clone the repository
git clone https://github.com/thevoxium/peek.git
cd peek

# Build and install
make
sudo make install

# Or compile manually
g++ src/main.cpp src/utils.cpp src/actions.cpp src/icons.cpp -o peek -Wall -Wextra -std=c++17 -lncurses
```

## Usage

### Basic Commands

```bash
# Browse current directory
peek

# Browse specific directory
peek /path/to/directory
```

### Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `↑/k` | Move up |
| `↓/j` | Move down |
| `l/Enter/→` | Open directory/file |
| `h/←` | Go to parent directory |
| `q` | Quit |

### File Operations

| Key | Action |
|-----|--------|
| `r` | Rename file/directory |
| `d` | Delete file/directory (with confirmation) |
| `y` | Copy full path to clipboard |

### Search & Navigation

| Key | Action |
|-----|--------|
| `/` | Enter search mode |
| `n` | Next search match |
| `N` | Previous search match |
| `e` | Exit search mode |

### Bookmarks

| Key | Action |
|-----|--------|
| `[` | Add current directory to bookmarks |
| `]` | Remove current directory from bookmarks |
| `b` | Show bookmarks list |

### Display Options

| Key | Action |
|-----|--------|
| `m` | Toggle sort by modified time |

## Configuration

Bookmarks are stored in `~/.peek_bookmarks` file.

## Development

### Compile with debugging

```bash
g++ src/main.cpp src/utils.cpp src/actions.cpp -o peek -Wall -Wextra -std=c++17 -lncurses -g
```

### Dependencies

- C++17
- ncurses
- Nerd Fonts (recommended for icons)

## License

MIT
