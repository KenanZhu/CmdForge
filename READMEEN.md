# CmdForge

English | [简体中文](README.md)

<p align="center">
  <img src="./doc/res/logo/CmdForge_Light.png" width="45%" alt="Light Logo">
  <img src="./doc/res/logo/CmdForge_Dark.png" width="45%" alt="Dark Logo">
</p>

<p align="center">
  <img src="https://img.shields.io/github/stars/KenanZhu/CmdForge.svg" alt="Stars">
  <img src="https://img.shields.io/github/issues/KenanZhu/CmdForge.svg" alt="Issues">
  <img src="https://img.shields.io/github/license/KenanZhu/CmdForge.svg" alt="License">
</p>

A command-line interface (CLI) construction framework that helps you rapidly develop cross-platform command-line tools.

## Project Overview

**CmdForge** is a C++ based framework for building command-line tools, assisting developers in quickly implementing CLI applications. The project provides complete sample code and documentation.

### Key Features

1. **Compact & Portable** - Single header & source file ^, no external dependencies
2. **Efficient Development** - Class method call-based construction, easy to start
3. **Comprehensive Features** - Supports subcommands, parameter configuration ^^, automatic help generation
4. **Cross-Platform** - Compatible with Windows, Linux, and macOS ^^^

*^ Requires creating wrapper function header files yourself (refer to development documentation or API.h in examples)*\
*^^ Not fully supported in current version*\
*^^^ Tested on specific platforms; functionality may vary*

### Usage

The `doc/` directory contains detailed development documentation explaining system architecture design and demo walkthroughs.

## Example Build

The `src/` directory includes:
- **CmdForge** library code in `lib`
- Changelog directory `log`
- Complete examples: `Demo.cpp` and `API.h`
  
These examples demonstrate **CmdForge**'s construction methods and runtime behavior.

### Requirements
- C++14+ compatible compiler
- CMake 3.15+ (recommend latest stable version)

### Build Steps
```bash
# Clone repository
git clone https://github.com/KenanZhu/CmdForge.git
cd CmdForge

# Create build directory
mkdir build
cd build

# Generate build system
cmake ..

# Compile project
cmake --build .
```

### Platform Builds

### Windows
- **Recommended Environment**: Visual Studio 2022 + CMake integration
- **Note**: Native CMD may not handle ANSI escape codes properly. Recommended terminals:
  - [Windows Terminal](https://aka.ms/terminal)
  - [MSYS2](https://www.msys2.org/) terminal
- **CMake Configuration Example**:
```bat
cmake -G "Visual Studio 14 2022" -A x64 ..
```

### Linux
- **Tested Environment**:
  - Ubuntu 24.10 (x86_64) with GCC 13.2
- **Dependencies**:
```bash
sudo apt-get install build-essential cmake
```

### macOS
- **Environment Setup**:
```bash
# Install Xcode command-line tools
xcode-select --install

# Install CMake via Homebrew
brew install cmake
```

## Documentation

Complete development documents in `doc/` directory:

| Document | Description |
|:-:|:-:|
|[Architecture Design Document](./doc/ArchDesign.md)|Framework module structure & demo details|
|[API Reference](./doc/APIGuide.md)|Core classes & methods documentation|

## Issue Reporting

Submit issues via [GitHub Issues](https://github.com/KenanZHu/CmdForge/issues). Please include:
1. OS version
2. Compiler version
3. Minimal reproducible code
4. Actual vs expected output

## License

Licensed under [GNU GPLv3](LICENSE). Free to use and modify with original copyright notice preserved.

## Contact

- **Maintainer**: KenanZhu (Nanoki)
- **Email**: <nanoki_zh@163.com>
- **Discussion**: Submit Issues or Pull Requests

---

_**Free to use** —— CmdForge_