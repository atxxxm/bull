# EN
Bull is a simple version control system.

## Commands

### `init`
Initializes a new Bull repository in the current directory. Creates the `.bull` directory, configures the base branch, and initializes the configuration files.

**Usage**: `bull init`

### `add`
Adds all non-ignored files in the current directory to the index for the next commit. Files specified in `.bullgnore' are excluded.

**Usage**: `bull add`

### `gnore`
Creates an empty `.bullgnore` file in the current directory to specify files or templates that Bull should ignore.

**Usage**: `bull gnore`

### `new <branch name>`
Creates a new branch with the specified name. The branch is added to the list of repository branches.

**Usage**: `bull new <branch name>`

### `branch`
Displays a list of all branches in the repository.

**Usage**: `bull branch`

### `status`
Shows the current branch, language settings, and changes in the working directory (added, modified, or deleted files). Compares the working directory with the last commit.

**Usage**: `bull status`

### `lang <language>`
Changes the language of Bull messages. Supported languages: `ru` (Russian) and `en` (English).

**Usage**: `bull lang <en|en>`

### `pack <commit message>`
Creates a new commit with the specified message. If the message ends with `.txt', the contents of the specified text file are used as the commit message.

**Usage**: `bull pack <commit message>`

### `unpack <hash_commit>`
Restores files from the specified commit (by hash) to the working directory.

**Usage**: `bull unpack <hash_commit>`

### `log`
Displays the commit history for the current branch. Outputs the hash and message of each commit.

**Usage**: `bull log`

### `set <branch name>`
Switches the current branch to the specified one.

**Usage**: `bull set <branch name>`

### `remove -b <branch name>`
Deletes the specified branch.

**Usage**: `bull remove -b <branch name>`

### `remove -c <hash_commit>`
Deletes the commit with the specified hash from the current branch.

**Usage**: `bull remove -c <hash_commit>`

### `reset`
Restores the working directory to the state of the last commit in the current branch.

**Usage**: `bull reset`


# RU
Bull — это простая система контроля версий.

## Команды

### `init`
Инициализирует новый репозиторий Bull в текущей директории. Создает директорию `.bull`, настраивает базовую ветку (`base`) и инициализирует конфигурационные файлы.

**Использование**: `bull init`

### `add`
Добавляет все неигнорируемые файлы в текущей директории в индекс для следующего коммита. Файлы, указанные в `.bullgnore` исключаются.

**Использование**: `bull add`

### `gnore`
Создает пустой файл `.bullgnore` в текущей директории для указания файлов или шаблонов, которые Bull должен игнорировать.

**Использование**: `bull gnore`

### `new <имя_ветки>`
Создает новую ветку с указанным именем. Ветка добавляется в список веток репозитория.

**Использование**: `bull new <имя_ветки>`

### `branch`
Отображает список всех веток в репозитории.

**Использование**: `bull branch`

### `status`
Показывает текущую ветку, языковые настройки и изменения в рабочей директории (добавленные, измененные или удаленные файлы). Сравнивает рабочую директорию с последним коммитом.

**Использование**: `bull status`

### `lang <язык>`
Изменяет язык сообщений Bull. Поддерживаемые языки: `ru` (русский) и `en` (английский).

**Использование**: `bull lang <ru|en>`

### `pack <сообщение_коммита>`
Создает новый коммит с указанным сообщением. Если сообщение заканчивается на `.txt`, содержимое указанного текстового файла используется как сообщение коммита.

**Использование**: `bull pack <сообщение_коммита>`

### `unpack <хэш_коммита>`
Восстанавливает файлы из указанного коммита (по хэшу) в рабочую директорию.

**Использование**: `bull unpack <хэш_коммита>`

### `log`
Отображает историю коммитов для текущей ветки. Выводит хэш и сообщение каждого коммита.

**Использование**: `bull log`

### `set <имя_ветки>`
Переключает текущую ветку на указанную.

**Использование**: `bull set <имя_ветки>`

### `remove -b <имя_ветки>`
Удаляет указанную ветку.

**Использование**: `bull remove -b <имя_ветки>`

### `remove -c <хэш_коммита>`
Удаляет коммит с указанным хэшем из текущей ветки.

**Использование**: `bull remove -c <хэш_коммита>`

### `reset`
Восстанавливает рабочую директорию до состояния последнего коммита в текущей ветке.

**Использование**: `bull reset`
