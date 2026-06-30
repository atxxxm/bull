# bull

A lightweight version control system written in C++17. Supports commits, branches, merging, diffing, stashing, and more.

---

## Table of Contents

- [Installation](#installation)
- [Building from source](#building-from-source)
- [Quick start](#quick-start)
- [Commands](#commands)
  - [Repository](#repository)
  - [Staging](#staging)
  - [Commits](#commits)
  - [Branches](#branches)
  - [Inspect](#inspect)
  - [Merge](#merge)
  - [Stash](#stash)
  - [Settings](#settings)
- [How it works](#how-it-works)
- [Project structure](#project-structure)
- [Requirements](#requirements)

---

## Installation

```bash
git clone https://github.com/atxxxm/bull
cd bull
cmake -B build -G Ninja
cmake --build build
```

The binary will be at `build/bull.exe` (Windows) or `build/bull` (Linux).

Optionally, add the `build/` directory to your `PATH` to use `bull` from anywhere.

---

## Quick start

```bash
# Initialize a repository
bull init

# Stage files
bull add .

# Create a commit
bull pack "first commit"

# Check status
bull status

# View commit history
bull log
```

---

## Commands

### Repository

| Command | Description |
|---------|-------------|
| `bull init` | Initialize a new repository in the current directory |
| `bull status` | Show current branch, language, and working tree changes |
| `bull log` | Show commit history with timestamps |
| `bull help` | Show all available commands |

---

### Staging

| Command | Description |
|---------|-------------|
| `bull add .` | Stage all files (respects `.bullgnore`) |
| `bull add <file> ...` | Stage specific files |
| `bull add -clean` | Clear the staging area |
| `bull gnore` | Create a `.bullgnore` file |

**`.bullgnore`** works like `.gitignore` — list files or directories to exclude from staging, one per line:

```
build
.vs
*.log
```

---

### Commits

| Command | Description |
|---------|-------------|
| `bull pack <message>` | Create a commit with staged files |
| `bull pack <file.txt>` | Create a commit using message from a `.txt` file |
| `bull unpack <hash>` | Restore working directory to a specific commit |
| `bull reset` | Restore working directory to the latest commit |
| `bull remove -c <hash>` | Delete a commit |

Each commit stores:
- A unique hash
- Timestamp
- Parent commit hash
- Commit message

Unchanged files between commits are stored as **references** (not copies), saving disk space.

---

### Branches

| Command | Description |
|---------|-------------|
| `bull new <name>` | Create a new branch |
| `bull branch` | List all branches (current branch highlighted) |
| `bull set <name>` | Switch to a branch |
| `bull remove -b <name>` | Delete a branch |

Example:
```bash
bull new feature
bull set feature
bull add .
bull pack "add feature"
bull set base
bull merge feature
```

---

### Inspect

| Command | Description |
|---------|-------------|
| `bull show <hash>` | Show file contents of a commit |
| `bull show -last` | Show file contents of the latest commit |
| `bull show -cur <file>` | Show a specific file from the latest commit |
| `bull list <hash>` | List files in a commit |
| `bull list -last` | List files in the latest commit |
| `bull diff` | Diff working tree vs latest commit |
| `bull diff <hash>` | Diff working tree vs a specific commit |
| `bull diff <hash1> <hash2>` | Diff between two commits |

`diff` output uses color-coded lines:

```
--- src/main.cpp
+ int newFunction() {
-     return 0;
+ return 1;
```

---

### Merge

| Command | Description |
|---------|-------------|
| `bull merge <branch>` | Merge a branch into the current branch |

When merging, bull compares the latest commits of both branches file by file:

- **New file** (only in source branch) — added to working directory
- **Identical file** — kept as is
- **Modified in both** — conflict markers are written into the file:

```
<<<<<<< base
old content
=======
new content from feature
>>>>>>> feature
```

After resolving conflicts, run `bull pack` to commit the merge result.

---

### Stash

| Command | Description |
|---------|-------------|
| `bull stash` | Save staged changes and restore last commit |
| `bull stash pop` | Restore saved changes from stash |

Useful when you need to quickly switch context without making a commit:

```bash
bull add .
bull stash          # save work in progress
bull set other      # switch branch
# ... do something ...
bull set base
bull stash pop      # restore saved changes
```

---

### Settings

| Command | Description |
|---------|-------------|
| `bull lang ru` | Switch output language to Russian |
| `bull lang en` | Switch output language to English |

---

## How it works

### Repository layout

```
.bull/
├── CONFIG          # current branch name
├── HEAD            # list of all branches
├── LANG            # output language setting
├── commit_list     # root commit list (unused after branching)
├── data_list.txt   # staging area (files queued for next commit)
├── stash/          # stash storage
└── <branch>/
    ├── commit_list         # commit history for this branch
    └── <hash>/
        ├── file_list       # files stored in this commit (full copies)
        ├── ref_list        # files unchanged from parent (stored as references)
        └── <files>...      # actual file contents
```

### Commit format

Each line in `commit_list`:
```
hash|timestamp|parent_hash|message
```

### Ref compression

When a file hasn't changed since the previous commit, bull writes an entry to `ref_list` instead of copying the file again:
```
filename|parent_hash
```

When restoring, bull follows the reference chain back to find the actual file content. This saves disk space for large projects with incremental changes.

### Diff algorithm

`bull diff` uses the **LCS (Longest Common Subsequence)** algorithm to compute line-by-line differences between files — the same approach used by traditional `diff` tools.

---

## Project structure

```
bull/
├── include/
│   ├── bull.hpp        # public API, constants, class declarations
│   └── slog.hpp        # logging library
├── src/
│   ├── main.cpp        # command dispatcher
│   ├── storage.cpp     # utility functions (hashing, parsing, file resolution)
│   ├── repo.cpp        # Init class (staging, branches, status)
│   ├── commit.cpp      # Action class (commits, diff, merge, stash)
│   └── slog.cpp        # logging implementation
└── CMakeLists.txt
```

---

## Requirements

- C++17 or later
- CMake 3.16+
- A C++ compiler (GCC, Clang, MSVC)
