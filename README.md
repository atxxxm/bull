## Bull — Lightweight and Intuitive Version Control System

Bull is a lightweight and intuitive version control system designed to manage changes in your projects. It is ideal for small teams and individual developers who need simplicity and efficiency.

## 🚀 Key Features

- **Project Initialization**: Sets up the structure for version control.
- **Version Saving**: Captures the project state with unique names.
- **File Ignoring**: Flexible configuration of ignored files via `.bullgnore`.
- **Version Viewing**: Easy access to a list of versions by names and IDs.
- **Version Restoration**: Quickly revert to a specific version by name or ID.
- **Change Analysis**: Tracks changes between the current state and saved versions.

## 📋 Commands

### `bull init`
Initializes a new project, creating a `.bull` directory for metadata storage.  

**Example**:  
`bull init`  

**Result**: Creates the `.bull` directory.

### `bull pack <name>`
Saves the current project version with the specified name.  

**Arguments**:  
`<name>` — Version name (required).  

**Example**:  
`bull pack v1.0`  

**Result**: Current version is saved as `v1.0`.

### `bull gnore`
Creates a `.bullgnore` file to specify ignored files and directories.  

**Example**:  
`bull gnore`  

**Result**: Creates a `.bullgnore` file in the project root.

### `bull get box name`
Lists all names of saved versions.  
**Example**:  

`bull get box name`  

**Result**:  
```
v1.0
v1.1
release-2.0
```

### `bull get box id`
Lists the IDs of saved versions.  

**Example**:  
`bull get box id`  

**Result**:  
```
9321323
3918021
5883012
```

### `bull get box full`
Lists all versions with their names and IDs. 

**Example**:  
`bull get box full`  

**Result**:  
```
v1.0 - 9321323
v1.1 - 3918021
release-2.0 - 5883012
```

### `bull unpack -id <id>`
Restores the project to the version specified by ID.  

**Arguments**:  
`<id>` — Version ID (required).  

**Example**:  
`bull unpack -id 5291023`  

**Result**: Project reverts to the version with ID `5291023`.

### `bull unpack last`
Restores the project to the last saved version.  

**Example**:  
`bull unpack last`  

**Result**: Project reverts to the latest version.

### `bull unpack <name>`
Restores the project to the version specified by name.  

**Arguments**:  
`<name>` — Version name (required).  

**Example**:  
`bull unpack v1.0`  

**Result**: Project reverts to version `v1.0`.

### `bull show change`
Displays changes between the current project state and the last saved version.  

**Example**:  
`bull show change`  

**Result**: Lists added, deleted, or modified files.

## 💡 Tips

- **Version Names**: Use clear and consistent names like `v1.0`, `release-2.0`.  
- **`.bullgnore` File**: Regularly update to exclude unnecessary files (e.g., temporary files or dependencies).  
- **Version Checking**: Use `bull get box full` before restoring to confirm the correct version name or ID.

## 🛠 Installation

1. Clone the repository:  
   `git clone https://github.com/atxxxm/bull.git`  
2. Grant execution permission:  
   `chmod +x install.sh`  
3. Run the script:  
   `./install.sh`  
4. Start using Bull in your project!

### 📄 License
Bull is distributed under the GNU General Public License v2.0.  
Note that, per the GPL-2.0 license, any derivative works must also be distributed under this license.


### ⚠️ **Warning**: Bull works **only on Linux**. Support for other operating systems is not yet implemented. Ensure you are using Linux before starting.
