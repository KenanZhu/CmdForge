# CmdForge

[English](README.md) | [中文](READMECN.md)

![CmdForge_Light_Logo](./doc/res/logo/CmdForge_Light.png#pic_center)
![CmdForge_Dark_Logo](./doc/res/logo/CmdForge_Dark.png#pic_center)

![STARS](https://img.shields.io/github/stars/KenanZHu/CmdForge.svg#pic_center) 
![ISSUES](https://img.shields.io/github/issues/KenanZHu/CmdForge.svg#pic_center) 
![LICENSE](https://img.shields.io/github/license/KenanZHu/CmdForge.svg#pic_center)

命令行工具（CLI）构建器 'CmdForge'，快速构建属于自己的命令行界面工具/软件。

## 简介

为了使 CLI 构建更加便捷，我们开发了 'CmdForge' 便携构建库，可以帮助你快速构建 CLI 工具/软件。\
在仓库中，我们提供了一个简易的 demo，你可以通过它来了解使用方法。

### 特性

1. **小巧便携**- 单一头文件 & 源文件，无需额外依赖。
2. **方便易用**- 类方法调用式的构建方式，快速上手。
3. **功能丰富**- 支持一般的命令行交互，支持各种选项自定义和参数自定义。
4. **平台兼容**- 支持 Windows 、Linux 、macOS ^ 等平台。

*^ 特定系统下进行测试，根据平台不同部分功能可能不支持。*

### 使用方法

在 {./doc} 文件夹中，你应该收到了详细的说明文档。此外，你可以通过我们提供的 demo 来了解使用方法。

### Demo的构建

截至到目前为止，我们已经测试过以下平台：

Windows: Windows 11 Pro [24H2] x86_64bits.\
Linux  : Ubuntu 24.10          x86_64bits. (虚拟机环境)

### Windows

我们使用 CMake ^ 来构建项目。所以你可以在 {./src}. 文件夹下，通过 CMake配置文件 {CMakeList.txt}\
 ，在已经安装 CMake 的情况下，在命令行终端输入以下命令来进行构建：

``mkdir build``\
``cd build``\
``cmake ..``\
``cmake --build .``

事实上，并不推荐你原生模式的终端来构建它，因为原生终端设置有时无法获取正确的键盘输入 ^^。我们同\
样推荐你使用 Visual Studio 2022 来构建它。

*^  如无特殊说明，项目中 CMake 特指 Windows : 4.0.0-rc3 Linux : 3.30.3 版本*\
*^^ 详情见说明文档*

### Linux

在 Linux 上，我们同样使用 CMake 来进行构建，因此这与 Windows 上 CMake 方法的构建方式是一样的。

### macOS

通常情况下，在 macOS 上的构建方式和 Linux 上是相似的，不过我们还没有进行测试。如果你遇到了问题，\
请提交 issue 来告知我们。

## 联系我们

Author : KenanZhu (Nanoki)\
Email  : <nanoki_zh@163.com>