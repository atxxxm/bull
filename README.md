# ENG
# Documentation for the Bull version control system

## Introduction

Bull is a simple version control system implemented in C++ for project management via the command line. It allows you to initialize projects, create "boxes" for storing versions of files and folders, unpack them, delete them, and track changes in files and directories.

## Installation

1. **Requirements**:
- C++ compiler (supporting the C++11 or higher standard).
   - Linux-based OS

2. **Build**:
- Make a file install.sh executed using the command: `chmod +x install.sh `
   - Run the file install.sh using the command: `./install.sh `

## Usage

Bull supports the following commands, which are passed through command line arguments:

### 1. Initializing the project (`init`)
Creates a '.bull` folder to store configuration and version data.

```bash
./bull init
```

- **Description**: Initializes a new project by creating a `.bull` directory.

### 2. Creating a new version (`pack`)
Collects files and folders from the current directory into a new "box" with a unique identifier and name.

```bash
./bull pack <box_name>
```

- **Description**: Creates a new version of the project, saving files and folders (except those ignored in `.bullgnore`).
- **Parameters**:
- `<box_name>`: The name of the box (cannot be empty).

### 3. Creating a ignore file (`gnore')
Creates a '.bullgnore` file to specify files and folders that should be ignored during packaging.

```bash
./bull gnore
```

- **Description**: Creates an empty `.bullgnore` file in the current directory.

### 4. List of boxes (`list`)
Displays a list of all boxes with their IDs and names.

```bash
./bull list
```

- **Result**: A list of boxes is displayed, for example:
``
  [BOX LIST]
  1: 1234567 - my_project
  2: 2345678 - another_project
  ```

###5. Unpacking the box (`unpack`)
Restores files and folders from the specified box to the current directory.

#### By the name of the box
```bash
./bull unpack <box_name>
```

- **Description**: Unpacks the box with the specified name.
- **Parameters**:
- `<box_name>`: The name of the box.

#### By Box ID
```bash
./bull unpack -id <box_id>
```

- **Description**: Unpacks the box with the specified ID.
- **Parameters**:
- `<box_id>`: The unique identifier of the box.

#### The last box
```bash
./bull unpack last
```

- **Description**: Unpacks the last created box.

### 6. Deleting a box (`delete')

#### By the name of the box
```bash
./bull delete -name <box_name>
```

- **Description**: Deletes the box with the specified name.
- **Parameters**:
- `<box_name>`: The name of the box.

#### By Box ID
```bash
./bull delete -id <box_id>
```

- **Description**: Deletes the box with the specified ID.
- **Parameters**:
- `<box_id>`: The unique identifier of the box.

#### The last box
```bash
./bull delete -last
```

- **Description**: Deletes the last created box.

### 7. Tracking changes (`change')

#### Folder changes
```bash
./bull change -dir
```

- **Description**: Compares the current folders with the folders in the last box and displays the differences (added or deleted folders).

#### Changes to files
```bash
./bull change -file
```

- **Description**: Compares the current files with the files in the last box and displays the differences (added or deleted files).

#### Changes in file contents
```bash
./bull change -data
```

- **Description**: Compares the contents of the files in the current directory with the contents of the files in the last box.

#### All changes
```bash
./bull change -all
```

- **Description**: Performs a comparison of folders, files, and their contents.

## Usage example

1. Initialization of the project:
   ```bash
   ./bull init
   ```

2. Creating the `.bullgnore` file:
``bash
./bull gnore
   echo "node_modules" > .bullgnore
   ```

3. Packing the project in a box:
   ```bash
   ./bull pack my_project
   ```

4. View the list of boxes:
   ```bash
   ./bull list
   ```

5. Unpacking the box:
   ```bash
   ./bull unpack my_project
   ```

6. Checking the changes:
   ```bash
   ./bull change -all
   ```

7. Removing the box:
   ```bash
   ./bull delete -name my_project
   ```

## Limitations and remarks

- Binary files (ELF) are automatically excluded from packaging.
- The `.bullgnore' file should contain relative paths to files and folders that should be ignored.

## License

Bull is distributed under the GNU General Public License v2.0 (GPL-2.0).






# RU
# Документация для системы контроля версий Bull

## Введение

Bull — это простая система контроля версий, реализованная на языке C++ для управления проектами через командную строку. Она позволяет инициализировать проекты, создавать "коробки" (box) для хранения версий файлов и папок, распаковывать их, удалять, а также отслеживать изменения в файлах и директориях.

## Установка

1. **Требования**:
   - Компилятор C++ (поддерживающий стандарт C++11 или выше).
   - ОС на базе Linux

2. **Сборка**:
   - Сделайте файл install.sh исполняемым с помошью команды: `chmod +x install.sh`
   - Запустите файл install.sh с помощью команды: `./install.sh`

## Использование

Bull поддерживает следующие команды, которые передаются через аргументы командной строки:

### 1. Инициализация проекта (`init`)
Создает папку `.bull` для хранения конфигурации и данных о версиях.

```bash
./bull init
```

- **Описание**: Инициализирует новый проект, создавая директорию `.bull`.

### 2. Создание новой версии (`pack`)
Собирает файлы и папки из текущей директории в новую "коробку" (box) с уникальным идентификатором и именем.

```bash
./bull pack <box_name>
```

- **Описание**: Создает новую версию проекта, сохраняя файлы и папки (кроме игнорируемых в `.bullgnore`).
- **Параметры**:
  - `<box_name>`: Имя коробки (не может быть пустым).

### 3. Создание файла игнорирования (`gnore`)
Создает файл `.bullgnore` для указания файлов и папок, которые должны игнорироваться при упаковке.

```bash
./bull gnore
```

- **Описание**: Создает пустой файл `.bullgnore` в текущей директории.

### 4. Список коробок (`list`)
Выводит список всех коробок с их ID и именами.

```bash
./bull list
```

- **Результат**: Выводится список коробок, например:
  ```
  [BOX LIST]
  1: 1234567 - my_project
  2: 2345678 - another_project
  ```

### 5. Распаковка коробки (`unpack`)
Восстанавливает файлы и папки из указанной коробки в текущую директорию.

#### По имени коробки
```bash
./bull unpack <box_name>
```

- **Описание**: Распаковывает коробку с указанным именем.
- **Параметры**:
  - `<box_name>`: Имя коробки.

#### По ID коробки
```bash
./bull unpack -id <box_id>
```

- **Описание**: Распаковывает коробку с указанным ID.
- **Параметры**:
  - `<box_id>`: Уникальный идентификатор коробки.

#### Последняя коробка
```bash
./bull unpack last
```

- **Описание**: Распаковывает последнюю созданную коробку.

### 6. Удаление коробки (`delete`)

#### По имени коробки
```bash
./bull delete -name <box_name>
```

- **Описание**: Удаляет коробку с указанным именем.
- **Параметры**:
  - `<box_name>`: Имя коробки.

#### По ID коробки
```bash
./bull delete -id <box_id>
```

- **Описание**: Удаляет коробку с указанным ID.
- **Параметры**:
  - `<box_id>`: Уникальный идентификатор коробки.

#### Последняя коробка
```bash
./bull delete -last
```

- **Описание**: Удаляет последнюю созданную коробку.

### 7. Отслеживание изменений (`change`)

#### Изменения в папках
```bash
./bull change -dir
```

- **Описание**: Сравнивает текущие папки с папками в последней коробке и выводит различия (добавленные или удаленные папки).

#### Изменения в файлах
```bash
./bull change -file
```

- **Описание**: Сравнивает текущие файлы с файлами в последней коробке и выводит различия (добавленные или удаленные файлы).

#### Изменения в содержимом файлов
```bash
./bull change -data
```

- **Описание**: Сравнивает содержимое файлов в текущей директории с содержимым файлов в последней коробке.

#### Все изменения
```bash
./bull change -all
```

- **Описание**: Выполняет сравнение папок, файлов и их содержимого.

## Пример использования

1. Инициализация проекта:
   ```bash
   ./bull init
   ```

2. Создание файла `.bullgnore`:
   ```bash
   ./bull gnore
   echo "node_modules" > .bullgnore
   ```

3. Упаковка проекта в коробку:
   ```bash
   ./bull pack my_project
   ```

4. Просмотр списка коробок:
   ```bash
   ./bull list
   ```

5. Распаковка коробки:
   ```bash
   ./bull unpack my_project
   ```

6. Проверка изменений:
   ```bash
   ./bull change -all
   ```

7. Удаление коробки:
   ```bash
   ./bull delete -name my_project
   ```

## Ограничения и замечания

- Бинарные файлы (ELF) автоматически исключаются из упаковки.
- Файл `.bullgnore` должен содержать относительные пути к файлам и папкам, которые нужно игнорировать.

## Лицензия

Bull распространяется под лицензией GNU General Public License v2.0 (GPL-2.0).
