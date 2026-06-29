# QCdot

![Static Badge](https://img.shields.io/badge/License-LGPL2.1-green)

[中文版本](README_zh.md)

A powerful visual programming engine and editor built with Qt6, featuring a node-based interface for dataflow programming.

## Features

- **Visual Programming**: Drag-and-drop node creation with intuitive canvas interface
- **Dual Execution Modes**: Dataflow and Signal-based execution
- **Built-in Node Library**: Math, logic, data, string, file I/O, and Qt UI nodes
- **Theme Support**: Dark and light themes with complete UI coverage
- **Internationalization**: Chinese/English bilingual UI
- **Cross-platform**: Windows, macOS, Linux (Qt6-based)

## Project Structure

```
QBlock/
├── engine/      # Core engine (Node, Port, Connection, Execution)
├── editor/      # Visual editor (Widgets, Scene, ThemeManager)
├── nodes/       # Built-in node implementations
└── examples/    # Example applications (QCdot-editor)
```

## Requirements

- Qt 6.5+ (Core, Gui, Widgets)
- CMake 3.20+
- C++17 compiler
- Ninja build system

## Building

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -G Ninja

# Build
ninja
```

## Running

```bash
# Run the example editor
./examples/QCdot-editor/QCdot-editor
```

## License

MIT License

Copyright © 2026. All rights reserved.
