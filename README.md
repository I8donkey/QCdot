# QBlock - Visual Programming Editor

![Static Badge](https://img.shields.io/badge/License-MIT-orange)

[中文](README_zh.md)

## Overview

QBlock is a powerful visual programming editor built on the Qt6 framework. It provides a node-based interface for creating programs through visual connections, making it ideal for rapid prototyping, educational purposes, and visual scripting.

## Features

- **Visual Node Programming**: Create programs by connecting nodes representing functions, variables, and UI components
- **Drag-and-Drop Interface**: Easy node creation from a categorized palette
- **Real-time Dataflow Execution**: Execute your program in dataflow or signal mode
- **Qt Widget Support**: Built-in nodes for creating Qt6 GUI applications
- **Bilingual UI**: Full support for Chinese and English interfaces
- **Theme Support**: Dark and light themes for comfortable editing
- **Save/Load**: Save your graphs as `.qcd` files and reload them later
- **Code Export**: Export your visual programs to C++ source code

## Installation

### Prerequisites

- Qt 6.5 or higher
- CMake 3.20 or higher
- C++17 compatible compiler (MSVC, MinGW, GCC)

### Build from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/qblock.git
cd qblock

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -G "MinGW Makefiles"  # or "Ninja" for faster builds

# Build
cmake --build . --config Release

# Run the editor
./examples/QCdot-editor/QCdot-editor.exe
```

## Quick Start

1. **Launch the Editor**: Run `QCdot-editor`
2. **Create Nodes**: Drag nodes from the left panel or double-click on empty canvas
3. **Connect Nodes**: Click and drag from an output port to an input port
4. **Edit Node Values**: Double-click on constant nodes to change their values
5. **Execute**: Press the play button to run in dataflow or signal mode

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+N` | New graph |
| `Ctrl+O` | Open file |
| `Ctrl+S` | Save file |
| `Ctrl+Shift+S` | Save as |
| `Delete` | Delete selected |
| `Ctrl+A` | Select all |
| `Ctrl++` | Zoom in |
| `Ctrl+-` | Zoom out |
| `Ctrl+Shift+F` | Fit to screen |

## Node Categories

- **Input**: Constants (int, float, bool, string), variables, counters
- **Output**: Print nodes
- **Container**: Main window, tab widget, layout
- **Widget**: Button, label, line edit, slider, checkbox, combo box, spin box, progress bar
- **Widget Method**: resize, move, show, hide, set enabled, set text, etc.
- **Math**: Add, subtract, multiply, divide, modulo, power, sqrt
- **Logic**: Equal, greater than, less than, and, or, not, if-else
- **String**: Concat, length, substring
- **Convert**: Type conversion nodes
- **File**: File read/write operations
- **Util**: Delay, random, etc.

## Project Structure

```
QBlock/
├── engine/          # Core execution engine
├── editor/          # Node editor UI components
├── nodes/           # Built-in node definitions
├── examples/        # Example applications
└── tests/           # Unit tests
```

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
