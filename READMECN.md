# CmdForge

[English](README.md) | [中文](READMECN.md)

![CmdForge_Light_Logo](./doc/CmdForge_Light.png#pic_center)
![CmdForge_Dark_Logo](./doc/CmdForge_Dark.png#pic_center)

![STARS](https://img.shields.io/github/stars/KenanZHu/CmdForge.svg#pic_center) 
![ISSUES](https://img.shields.io/github/issues/KenanZHu/CmdForge.svg#pic_center) 
![LICENSE](https://img.shields.io/github/license/KenanZHu/CmdForge.svg#pic_center)

命令行工具构建器，快速构建属于自己的命令行界面工具

## 简介

为了使CLI构建更加便捷，'CmdForge'可以帮助你快速构建CLI工具。
在仓库中，我们提供了一个demo，你可以通过它来获取使用方法。

### 特性

1. 快速地 构建你的CLI工具。
2. 简单地 使用构建逻辑。
3. 广泛地 支持平台。

### 使用

在 {./doc} 文件夹中，你应该收到了详细的指令文档。此外，你可以通过我们提供的demo来获取使用方法。

## 构建

截至到目前为止，我们已经测试过以下平台：\

Windows: Windows 11 Pro [24H2] x86_64bits.
Linux  : Ubuntu 24.10          x86_64bits. (虚拟机环境)

### Windows

我们使用CMake来构建项目。所以你可以使用以下命令来构建它，通过文件 {CMakeList.txt}

``mkdir build``\
``cd build``\
``cmake ..``\
``cmake --build .``

事实上，原生模式的终端并不推荐你来构建它，因为原生终端设置无法获取正确的键盘输入。

我们同样推荐你使用Visual Studio 2022来构建它。

### Linux

和Windows上使用CMake的构建方式相同 {./src}.

``mkdir build``\
``cd build``\
``cmake ..``\
``cmake --build .``

### MacOS

通常情况下，在MacOS上的构建方式和Linux上是相似的，不过我们还没有进行测试。如果你遇到了问题，请提交issue来告知我们。

## 联系我

Author : KenanZhu (Nanoki)\
Email  : <nanoki_zh@163.com>