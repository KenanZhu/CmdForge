# CmdForge

[English](README.md) | [中文](READMECN.md)

![CmdForge_Light_Logo](./doc/CmdForge_Light.png#pic_center)
![CmdForge_Dark_Logo](./doc/CmdForge_Dark.png#pic_center)

![STARS](https://img.shields.io/github/stars/KenanZHu/CmdForge.svg#pic_center) 
![ISSUES](https://img.shields.io/github/issues/KenanZHu/CmdForge.svg#pic_center) 
![LICENSE](https://img.shields.io/github/license/KenanZHu/CmdForge.svg#pic_center)

Command-Line software builder which can fastly build CLI tool own by yourself.

## Brief

To make CLI more accessible, 'CmdForge' can help you to build CLI tool/software
easily.In the repository, we offer one demo which you can get the usage.

### Features

1. Fastly shape your CLI tool.
2. Easily use, builde logic.
3. Widely support platform.

### Usage

In the {./doc} folder, you should received a detailed instruction document. Also,
you can get the usage by the demo we offerred.

## Build

By now, we have only tested on:\

Windows: Windows 11 Pro [24H2] x86_64bits.
Linux  : Ubuntu 24.10          x86_64bits. (On virtual machine)

### Windows

We use CMake to build the project. So you can use the following command to build
it by file {CMakeList.txt} in folder {./src}.

``mkdir build``\
``cd build``\
``cmake ..``\
``cmake --build .``

In fact, the original terminal is not recommended to build it, because the termial
set is not albe to get keyboard input.

We also recommend you build it by Visual Studio 2022.

### Linux

Its the same way to build by CMake on the Windows {./src}.

``mkdir build``\
``cd build``\
``cmake ..``\
``cmake --build .``

### MacOS

In general, the build of MacOS is simliar to Linux. We haven't test it. If you 
encounter any problem,please submit an issue to let us know.

## Contact

Author : KenanZhu (Nanoki)\
Email  : <nanoki_zh@163.com>