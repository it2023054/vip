<p align="center">
    <img src="assets/logo.png" alt="VIP Logo" width="200">
</p>

<p align="center">Vi rIPoff — A modal terminal-based text editor in C</p>

---

VIP is a modal, terminal-based text editor written in C. It is inspired by Vim and implements a subset of its modal editing model, including normal, insert, visual, and command modes.

It is built from scratch using POSIX terminal APIs without external UI libraries. The project demonstrates systems programming concepts such as raw terminal handling, dynamic memory management, state machines, and command parsing.

---

## Features

- Modal editing (Normal, Insert, Visual, Command)
- Vim-like operator + motion commands
- Dynamic text buffer implementation
- Cursor navigation
- File loading and saving
- Terminal raw mode with alternate screen buffer
- State-machine based input parser
- Basic status line and command-line interface

---

## Documentation

A full technical description of the system architecture, data structures, and command parsing design is available in the project paper:

[VIP Project Paper (PDF)](docs/vip_paper.pdf)

---

## Build Instructions

### Requirements

- GCC or Clang
- Make
- POSIX-compatible system

### Build (Debug - default)


```bash
make
```

or explicitly:

```bash
make debug
```

This produces the debug executable:

```bash
bin/debug/vip
```

### Build (Release)

```bash
make release
```

This produces the optimized binary:

```bash
bin/release/vip
```

---

## Running the Editor

You can run VIP in two ways:

## Option 1 - Run using Make

```bash
make run
```

or

```bash
make run ARGS="<filename>"
```

## Option 2 - Run the binary directly

Debug build:

```bash
./bin/debug/vip
```

or

```bash
./bin/debug/vip <filename>
```

Release build:

```bash
./bin/release/vip
```

or

```bash
./bin/release/vip <filename>
```

---

## Debugging

Run with GDB:

```bash
make gdb
```

---

## Clean Build Files

```bash
make clean
```
