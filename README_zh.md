# QCdot

![Static Badge](https://img.shields.io/badge/License-LGPL2.1-green)

[English Version](README.md)

一个基于 Qt6 构建的强大可视化编程引擎和编辑器，采用节点式界面进行数据流编程。

## 功能特性

- **可视化编程**：拖放式节点创建，直观的画布界面
- **双执行模式**：数据流和信号两种执行模式
- **内置节点库**：数学运算、逻辑、数据、字符串、文件 I/O 和 Qt UI 节点
- **主题支持**：深色和浅色主题，覆盖完整 UI
- **国际化**：中英双语界面
- **跨平台**：Windows、macOS、Linux（基于 Qt6）

## 项目结构

```
QBlock/
├── engine/      # 核心引擎（节点、端口、连接、执行）
├── editor/      # 可视化编辑器（控件、场景、主题管理）
├── nodes/       # 内置节点实现
└── examples/    # 示例应用（QCdot-editor）
```

## 环境要求

- Qt 6.5+（Core、Gui、Widgets）
- CMake 3.20+
- C++17 编译器
- Ninja 构建系统

## 构建步骤

```bash
# 创建构建目录
mkdir build && cd build

# 使用 CMake 配置
cmake .. -G Ninja

# 构建
ninja
```

## 运行

```bash
# 运行示例编辑器
./examples/QCdot-editor/QCdot-editor
```

## 许可证

MIT License

Copyright © 2026. All rights reserved.
