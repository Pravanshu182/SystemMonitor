# C++ System Monitor Tool

## Overview

This is a professional terminal-based system monitor tool built with C++ and ncurses. The project provides a real-time display of system and process statistics in a clean, organized, and interactive UI. You can view system uptime, memory usage, inspect running processes, sort processes by different columns, and send signals to terminate them.

## Features

- Professional ncurses UI with header bar, status bar, and window borders
- Real-time updated system information (uptime, memory)
- Tabular process list with PID, user, state, memory usage, percent, and command
- Color-highlighted selection in the process list
- Sorting by PID, memory, and (placeholder for CPU)
- Interactive process selection and signal-sending (kill)
- Controls displayed at the bottom at all times

## Dependencies

- Linux OS with `/proc` filesystem (tested on Ubuntu/Debian)
- C++17 compatible compiler (e.g., `g++`)
- ncurses library

## Installation

Install dependencies on Ubuntu/Debian:

- sudo apt update
- sudo apt install g++ libncurses5-dev libncursesw5-dev


## Build

From project root directory, run:
-make

## Usage

- ./system_monitor

## Controls

| Key        | Action                              |
|------------|-------------------------------------|
| P/p        | Sort by PID                         |
| C/c        | Sort by CPU                        |
| M/m        | Sort by memory                      |
| Up/Down    | Move process selection              |
| K/k        | Kill selected process (send SIGTERM)|
| Q/q        | Quit the program                    |

## File Structure

- `src/main.cpp`            — main application UI and logic
- `src/system_info.cpp`     — system and process info functions
- `include/system_info.h`   — function prototypes and structures
- `Makefile`                — build instructions

