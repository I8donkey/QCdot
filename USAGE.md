# QBlock - Usage Guide

## Version

Current version: 1.0.0

## Basic Concepts

### Nodes

Nodes are the basic building blocks in QBlock. Each node represents a function, variable, or UI component. Nodes have:
- **Input ports**: Left side of the node (accepting data)
- **Output ports**: Right side of the node (producing data)
- **Title**: Name of the node type

### Connections

Connections link output ports to input ports, creating a data flow. Connections are color-coded by data type.

### Data Types

- **Integer**: Whole numbers (1, 2, 3, ...)
- **Float**: Decimal numbers (1.5, 3.14, ...)
- **Boolean**: True/False values
- **String**: Text values
- **Generic**: Any data type

## Node Palette

The left panel shows all available nodes, organized by category:

1. **Input**: Constants and variable nodes
2. **Output**: Print and display nodes
3. **Container**: Window and layout nodes
4. **Widget**: UI component nodes
5. **Widget Method**: Widget manipulation nodes
6. **Math**: Arithmetic operations
7. **Logic**: Comparison and boolean operations
8. **String**: String manipulation
9. **Convert**: Type conversion
10. **File**: File operations
11. **Util**: Utilities (delay, random, etc.)

### Creating Nodes

1. **Drag and drop**: Drag a node from the palette to the canvas
2. **Inline picker**: Drag a connection wire to empty space and select a node type
3. **Double-click**: Double-click on empty canvas to open the node picker

### Connecting Nodes

1. Click on an output port (right side)
2. Drag to an input port (left side)
3. Release to create connection

### Deleting Nodes

1. Select the node(s) to delete
2. Press `Delete` key or click the delete button

### Editing Node Values

Double-click on a constant node to edit its value.

## Execution Modes

### Dataflow Mode (Default)

Nodes execute when all their inputs have valid values. Execution flows automatically based on data availability.

### Signal Mode

Nodes execute from left to right, following explicit signal connections. Useful for imperative logic.

## File Operations

### Saving

1. `Ctrl+S` or File → Save
2. Choose a location and filename
3. Graph saves as `.qcd` file

### Opening

1. `Ctrl+O` or File → Open
2. Select a `.qcd` file
3. Graph loads into the editor

### Exporting to C++

1. File → Export as C++
2. Choose a location for the generated source code

## Theme and Language

### Switching Themes

Use the toolbar buttons or menu options to switch between dark and light themes.

### Switching Languages

Use Language menu to switch between English and Chinese (中文).

## Advanced Features

### Variable Nodes

Use Variable nodes to store and retrieve values across the graph:

1. Create a Variable node
2. Connect to read or write values
3. Variables persist across executions

### Conditional Execution

Use IfElse nodes to branch execution based on conditions:

1. Add an IfElse node
2. Connect a boolean condition to the "condition" input
3. Connect nodes to "true" and "false" branches

### Container Nodes

Use container nodes to create structured UI layouts:

1. Add a MainWindow or TabWidget container
2. Add a Layout node
3. Add widgets and connect them to the layout
4. Use method nodes to manipulate containers

## Troubleshooting

### Connection won't create

- Check that the data types are compatible
- Check that you're connecting output to input (not output to output)

### Node doesn't execute

- Ensure all required inputs are connected
- Check for circular dependencies in signal mode

### Application crashes

- Save your work frequently
- Check the console output for error messages

## API Reference

### Node Types

All node types are defined in `nodes/include/QBlock/BuiltinNodes.h`.

### Engine API

Core execution engine is in `engine/include/QBlock/`.

### Editor API

Editor components are in `editor/include/QBlock/`.
