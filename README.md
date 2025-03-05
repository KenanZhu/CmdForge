# CmdForge

[English](README.md) | [中文](READMECN.md)

![CmdForge_Light_Logo](./doc/res/logo/CmdForge_Light.png#pic_center)
![CmdForge_Dark_Logo](./doc/res/logo/CmdForge_Dark.png#pic_center)

![STARS](https://img.shields.io/github/stars/KenanZHu/CmdForge.svg#pic_center) 
![ISSUES](https://img.shields.io/github/issues/KenanZHu/CmdForge.svg#pic_center) 
![LICENSE](https://img.shields.io/github/license/KenanZHu/CmdForge.svg#pic_center)

Command Line Interface (CLI) tool/software builder 'CmdForge', which can fastly build CLI\
tool/software own by yourself.

## Brief

To make CLI more convenient, we develop a portable building library 'CmdForge', which can \
help you fastly build CLI tool/software. In the repository, we provide a simple demo, you \
can use it to learn how to use it. 

### Features

1. **Compact and Portable** - Single header & source file, no additional dependencies.
2. **User-Friendly** - Object-oriented method call-style setup for rapid onboarding.
3. **Feature-Rich** - Supports common CLI interactions, customizable options, and parameter definitions.
4. **Cross-Platform Compatibility** - Compatible with Windows, Linux, macOS^, and other platforms

*^ Tested on specific system, some functions maybe not supportted rest on different platform.*

### Usage

In the {./doc} folder, you should received a detailed instruction document. Also, you can \
get the usage by the demo we offerred.

## Build of our demo

By now, we have only tested on:

Windows: Windows 11 Pro [24H2] x86_64bits.\
Linux  : Ubuntu 24.10          x86_64bits. (On virtual machine)

### Windows

We use CMake^ to build the project. In the ./src folder, using the CMake configuration file\
CMakeLists.txt, you can build the project via the command-line terminal by running the following\
 commands (assuming CMake is already installed):

``mkdir build``\
``cd build``\
``cmake ..``\
``cmake --build .``

However, using the native terminal for building is not recommended, as it may sometimes fail to \
capture keyboard input correctly^^. We recommend using Visual Studio 2022 for building instead.

*^  Unless otherwise specified, "CMake" in this project refers to version 4.0.0-rc3 on Windows and 3.30.3 on Linux.*\
*^^ See the documentation for details.*

### Linux

On Linux, we also use CMake for building, which follows the same process as the CMake method \
described for Windows.

### macOS

The build process on macOS is generally similar to Linux. However, this has not been thoroughly\
tested. If you encounter issues, please submit an issue to notify us.

## Contact

Author : KenanZhu (Nanoki)\
Email  : <nanoki_zh@163.com>