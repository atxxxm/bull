#include "../include/bull.hpp"
#include "../include/slog.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <ctime>
#include <vector>
#include <algorithm>

extern slog::LOG log_;

std::string bull::Action::hash_gen()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);
    std::uniform_int_distribution<> ch('a', 'z');

    std::string hash;
    std::string cur_branch = bull::current_branch();

    do
    {
        hash.clear();
        for (int i = 0; i < 8; i++)
        {
            hash += std::to_string(dist(gen));
            hash += static_cast<char>(ch(gen));
        }
    }
    while (std::filesystem::exists(bull::init_dir + "/" + cur_branch + "/" + hash));

    return hash;
}

bool bull::Action::isMoreOne()
{
    int i = 0;
    std::string path = bull::init_dir + "/" + bull::branch_list, line;

    std::ifstream read_head(path);

    while (std::getline(read_head, line))
    {
        if (line.empty()) continue;
        i++;
    }

    read_head.close();

    return (i > 1);
}

std::string bull::Action::select_random_branch(const std::string& exclude)
{
    std::string line, path = bull::init_dir + "/" + bull::branch_list;

    std::ifstream read_head(path);

    while (std::getline(read_head, line))
    {
        if (line.empty() || line == exclude) continue;
        return line;
    }

    return "base";
}

void bull::Action::pack(const std::string& comm)
{
    std::string lang = bull::getCurrentLang();

    if (!bull::isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string commit, line_comm;

    if (comm.size() > 4 && comm.substr(comm.size() - 4) == ".txt")
    {
        std::ifstream read_commit(comm);

        if (!read_commit.is_open())
        {
            if (lang == "ru") log_.ERROR("Некорректный путь к файлу .txt!");
            else log_.ERROR("Incorrect file path .txt!");
            return;
        }

        while (std::getline(read_commit, line_comm))
        {
            commit += line_comm + "\n";
        }

        read_commit.close();
    }
    else commit = comm;

    std::string data_path_check = bull::init_dir + "/" + bull::data_list;
    std::ifstream check_staging(data_path_check);
    bool staging_empty = true;
    std::string check_line;
    while (std::getline(check_staging, check_line))
    {
        if (!check_line.empty()) { staging_empty = false; break; }
    }
    check_staging.close();

    if (staging_empty)
    {
        if (lang == "ru") log_.WARNING("Нет файлов в staging area. Используйте 'bull add'.");
        else log_.WARNING("Nothing to commit. Use 'bull add' to stage files.");
        return;
    }

    std::string hash = hash_gen();
    std::string cur_branch = bull::current_branch();
    std::string path = bull::init_dir + "/" + cur_branch + "/" + bull::commit_list;

    std::string parent_hash = bull::getLastCommit();
    if (parent_hash.empty()) parent_hash = "none";

    std::string timestamp = std::to_string(std::time(nullptr));

    std::string commit_list, lemma;
    std::ifstream read_commit_list(path);

    while (std::getline(read_commit_list, lemma))
    {
        if (lemma.empty()) continue;
        commit_list += lemma + "\n";
    }

    read_commit_list.close();

    std::ofstream tocommit(path);
    tocommit << hash << "|" << timestamp << "|" << parent_hash << "|" << commit << "\n" << commit_list;
    tocommit.close();

    std::string path_to_copy = bull::init_dir + "/" + cur_branch + "/" + hash + "/";
    std::filesystem::create_directory(bull::init_dir + "/" + cur_branch + "/" + hash);

    std::string data_path = bull::init_dir + "/" + bull::data_list;
    std::string line;
    std::ifstream get_list_file(data_path);
    std::ofstream wrt;
    std::ifstream red;
    std::ofstream list_f(path_to_copy + bull::file_list);

    while (std::getline(get_list_file, line))
    {
        if (line.empty()) continue;
        if (!std::filesystem::exists(line)) continue;

        std::string normalized = line;
        if (normalized.size() >= 2 && normalized[0] == '.' && normalized[1] == '/')
            normalized = normalized.substr(2);

        list_f << normalized << "\n";

        std::string target_file = path_to_copy + normalized;
        std::string target_dir = std::filesystem::path(target_file).parent_path().string();

        if (!target_dir.empty()) std::filesystem::create_directories(target_dir);

        red.open(line, std::ios::binary);
        wrt.open(target_file, std::ios::binary);

        if (red.is_open() && wrt.is_open())
        {
            wrt << red.rdbuf();
        }
        else
        {
            if (lang == "ru") log_.ERROR_NE("Не удалось упаковать файл: %s", line.c_str());
            else log_.ERROR_NE("Failed to pack file: %s", line.c_str());
        }

        wrt.close();
        red.close();
    }

    get_list_file.close();
    list_f.close();

    log_.CUSTOM_NSL("green", "+ " + cur_branch + "->" + hash + " - " + commit);
}

void bull::Action::unpack(const std::string& hash)
{
    std::string lang = bull::getCurrentLang();
    if (!bull::is_commit(hash))
    {
        if (lang == "ru") log_.ERROR_NE("Коммит с хэшем '%s' не существует!", hash.c_str());
        else log_.ERROR_NE("The commit with the hash '%s' does not exist!", hash.c_str());
        return;
    }

    std::string cur_branch = bull::current_branch();
    std::string path = bull::init_dir + "/" + cur_branch + "/" + hash + "/" + bull::file_list;
    std::string path_to_read = bull::init_dir + "/" + cur_branch + "/" + hash + "/";

    std::string line, fmt_dir;
    std::ifstream r_file_list(path);
    std::ifstream red;
    std::ofstream wrt;

    while (std::getline(r_file_list, line))
    {
        if (line.empty()) continue;
        if (!std::filesystem::exists(path_to_read + line)) continue;

        fmt_dir = std::filesystem::path(line).parent_path().string();
        if (!fmt_dir.empty()) std::filesystem::create_directories(fmt_dir);

        red.open(path_to_read + line, std::ios::binary);
        wrt.open(line, std::ios::binary);

        if (red.is_open() && wrt.is_open()) wrt << red.rdbuf();

        red.close();
        wrt.close();
    }

    r_file_list.close();

    if (lang == "ru") log_.INFO_NE("Коммит '%s' успешно распакован", hash.c_str());
    else log_.INFO_NE("The commit '%s' has been successfully unpacked", hash.c_str());
}

void bull::Action::set(const std::string& branch_name)
{
    std::string lang = bull::getCurrentLang();
    if (!bull::isValidName(branch_name))
    {
        if (lang == "ru") log_.ERROR_NE("Недопустимое имя ветки: '%s'", branch_name.c_str());
        else log_.ERROR_NE("Invalid branch name: '%s'", branch_name.c_str());
        return;
    }
    if (!bull::is_branch(branch_name))
    {
        if (lang == "ru") log_.ERROR_NE("Ветка '%s' не существует!", branch_name.c_str());
        else log_.ERROR_NE("Branch '%s' does not exist!", branch_name.c_str());

        std::string fmt = lang == "ru"
            ? "Хотите создать ветку '" + branch_name + "'? (Y/N): "
            : "Do you want to create a '" + branch_name + "'? (Y/N): ";
        log_.CUSTOM_NSL_WITHOUT_N("purple", fmt);

        char answer;
        std::cin >> answer;

        if (answer == 'Y' || answer == 'y')
        {
            bull::Init init;
            init.branch(branch_name);
        }
        else return;
    }

    std::string path = bull::init_dir + "/" + bull::config;
    std::ofstream wrt_branch(path);
    wrt_branch << (lang == "ru" ? "[ТЕКУЩАЯ ВЕТКА]\n" : "[CURRENT BRANCH]\n") << branch_name;
    wrt_branch.close();

    if (lang == "ru") log_.CUSTOM("light_blue", "ТЕКУЩАЯ ВЕТКА", branch_name);
    else log_.CUSTOM("light_blue", "CURRENT BRANCH", branch_name);
}

void bull::Action::remove_branch(const std::string& branch_name)
{
    std::string lang = bull::getCurrentLang();
    std::string cur_branch = bull::current_branch();

    if (!bull::isValidName(branch_name))
    {
        if (lang == "ru") log_.ERROR_NE("Недопустимое имя ветки: '%s'", branch_name.c_str());
        else log_.ERROR_NE("Invalid branch name: '%s'", branch_name.c_str());
        return;
    }
    if (!bull::is_branch(branch_name))
    {
        if (lang == "ru") log_.ERROR("Такой ветки не существует!");
        else log_.ERROR("There is no such branch!");
        return;
    }

    if (branch_name == cur_branch)
    {
        if (lang == "ru") log_.WARNING("Вы не можете удалить текущую ветку, так как находитесь на ней!");
        else log_.WARNING("You cannot delete the current branch because you are currently on it!");

        if (!isMoreOne()) return;

        std::string fmt = lang == "ru"
            ? "Переключиться автоматически на другую ветку? (Y/N): "
            : "Will it switch to another branch automatically? (Y/N): ";
        log_.CUSTOM_NSL_WITHOUT_N("purple", fmt);

        char answer;
        std::cin >> answer;

        if (answer == 'Y' || answer == 'y')
        {
            set(select_random_branch(branch_name));
        }
        else return;
    }

    std::string path = bull::init_dir + "/" + bull::branch_list;
    std::string line, new_branch_list;

    std::ifstream read_branch_list(path);

    while (std::getline(read_branch_list, line))
    {
        if (branch_name == line || line.empty()) continue;
        new_branch_list += line + "\n";
    }

    read_branch_list.close();

    std::ofstream write_branch_list(path);
    write_branch_list << new_branch_list << "\n";
    write_branch_list.close();

    std::filesystem::remove_all(bull::init_dir + "/" + branch_name);

    log_.CUSTOM_NSL("red", "-> - " + branch_name);
}

void bull::Action::remove_commit(const std::string& commit_hash)
{
    std::string lang = bull::getCurrentLang();
    std::string cur_branch = bull::current_branch();
    std::string path = bull::init_dir + "/" + cur_branch + "/" + bull::commit_list;

    if (!bull::is_commit(commit_hash))
    {
        if (lang == "ru") log_.ERROR_NE("Коммит с хэшем '%s' не найден!", commit_hash.c_str());
        else log_.ERROR_NE("No commits found with the hash '%s'", commit_hash.c_str());
        return;
    }

    std::ifstream read_commit_list(path);
    int del_line = bull::getLineCommit(cur_branch, commit_hash), i = 0;
    std::string line, new_commit_list;

    while (std::getline(read_commit_list, line))
    {
        ++i;
        if (del_line == i) continue;
        new_commit_list += line + "\n";
    }

    read_commit_list.close();

    std::ofstream write_commit_list(path);
    write_commit_list << new_commit_list;
    write_commit_list.close();

    std::filesystem::remove_all(bull::init_dir + "/" + cur_branch + "/" + commit_hash);

    log_.CUSTOM_NSL("red", "- " + cur_branch + "->" + commit_hash);
}

void bull::Action::log()
{
    std::string lang = bull::getCurrentLang();
    if (!bull::isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string branch = bull::current_branch();
    std::string path_to_branch = bull::init_dir + "/" + branch + "/" + bull::commit_list;

    std::ifstream read_commites(path_to_branch);
    if (!read_commites.is_open()) return;

    if (lang == "ru") log_.CUSTOM_NSL("green", "ветка -> " + branch + "\n");
    else log_.CUSTOM_NSL("green", "branch -> " + branch + "\n");

    std::string line;
    bool first = true;

    while (std::getline(read_commites, line))
    {
        if (line.empty()) continue;

        bull::CommitInfo info = bull::parseCommitLine(line);

        std::time_t ts = static_cast<std::time_t>(std::stoll(info.timestamp));
        std::tm* t = std::localtime(&ts);
        char date_buf[32];
        std::strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M:%S", t);

        std::string entry = info.hash + " [" + date_buf + "] " + info.message;

        if (first) { log_.CUSTOM_NSL("orange", entry); first = false; }
        else printf("%s\n", entry.c_str());
    }

    read_commites.close();
}

void bull::Action::reset()
{
    std::string lang = bull::getCurrentLang();
    if (!bull::isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string hash = bull::getLastCommit();

    if (hash.empty())
    {
        if (lang == "ru") log_.ERROR("У вас нет коммитов!");
        else log_.ERROR("You don't have any commits!");
        return;
    }

    unpack(hash);
}

void bull::Action::show_func(const std::string& commit_hash)
{
    std::string lang = bull::getCurrentLang();
    if (!bull::is_commit(commit_hash))
    {
        if (lang == "ru") log_.ERROR_NE("Коммит с хэшем '%s' не найден!", commit_hash.c_str());
        else log_.ERROR_NE("No commits found with the hash '%s'", commit_hash.c_str());
        return;
    }

    std::string cur_branch = bull::current_branch();
    std::string path_to_file_list = bull::init_dir + "/" + cur_branch + "/" + commit_hash + "/" + bull::file_list;
    std::string path_to_commit = bull::init_dir + "/" + cur_branch + "/" + commit_hash + "/";

    std::ifstream read_to_file_list(path_to_file_list);
    std::ifstream read_file;
    std::string line;

    log_.CUSTOM_NSL("green", cur_branch + "->" + commit_hash);

    while (std::getline(read_to_file_list, line))
    {
        if (line.empty()) continue;
        if (bull::isBinaryFile(path_to_commit + line)) continue;

        read_file.open(path_to_commit + line, std::ios::binary);

        log_.CUSTOM_NSL("blue", line + ":");
        std::cout << std::string(std::istreambuf_iterator<char>(read_file), std::istreambuf_iterator<char>()) << std::endl;
        printf("\n");

        read_file.close();
    }

    read_to_file_list.close();
}

void bull::Action::show(const std::string& commit_hash)
{
    if (commit_hash.empty()) return;
    show_func(commit_hash);
}

void bull::Action::show_last()
{
    std::string commit = bull::getLastCommit();
    if (commit.empty()) return;
    show_func(commit);
}

void bull::Action::show_cur(const std::string& filename)
{
    std::string lang = bull::getCurrentLang();
    std::string cur_branch = bull::current_branch();
    std::string commit = bull::getLastCommit();
    std::string path = bull::init_dir + "/" + cur_branch + "/" + commit + "/" + filename;

    std::ifstream read_file(path, std::ios::binary);

    if (!read_file.is_open())
    {
        std::string res = cur_branch + "->" + commit;
        if (lang == "ru") log_.ERROR_NE("Файл '%s' в %s не найден!", filename.c_str(), res.c_str());
        else log_.ERROR_NE("The file '%s' was not found in the %s!", filename.c_str(), res.c_str());
        return;
    }

    log_.CUSTOM_NSL("blue", filename + ":");

    std::string line;
    while (std::getline(read_file, line))
    {
        printf("%s\n", line.c_str());
    }

    read_file.close();
}

void bull::Action::comm_list_func(const std::string& commit_hash)
{
    std::string lang = bull::getCurrentLang();
    if (!bull::is_commit(commit_hash))
    {
        if (lang == "ru") log_.ERROR_NE("Коммит с хэшем '%s' не найден!", commit_hash.c_str());
        else log_.ERROR_NE("No commits found with the hash '%s'", commit_hash.c_str());
        return;
    }

    std::string cur_branch = bull::current_branch();
    std::string path = bull::init_dir + "/" + cur_branch + "/" + commit_hash + "/" + bull::file_list;

    std::ifstream read_fl(path);
    std::string line;

    log_.CUSTOM_NSL("green", cur_branch + "->" + commit_hash + "\n");

    if (lang == "ru") log_.CUSTOM("blue", "ФАЙЛЫ", "");
    else log_.CUSTOM("blue", "FILES", "");

    while (std::getline(read_fl, line))
    {
        printf("%s\n", line.c_str());
    }

    read_fl.close();
}

void bull::Action::comm_list(const std::string& commit_hash)
{
    comm_list_func(commit_hash);
}

void bull::Action::comm_list_last()
{
    comm_list_func(bull::getLastCommit());
}

static std::vector<std::string> read_lines(const std::string& path)
{
    std::vector<std::string> lines;
    std::ifstream f(path);
    std::string line;
    while (std::getline(f, line)) lines.push_back(line);
    return lines;
}

void bull::Action::diff_files(const std::string& path_a, const std::string& path_b, const std::string& filename)
{
    auto a = read_lines(path_a);
    auto b = read_lines(path_b);

    int m = static_cast<int>(a.size());
    int n = static_cast<int>(b.size());

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
    for (int i = 1; i <= m; i++)
        for (int j = 1; j <= n; j++)
            dp[i][j] = (a[i-1] == b[j-1]) ? dp[i-1][j-1] + 1 : std::max(dp[i-1][j], dp[i][j-1]);

    std::vector<std::pair<char, std::string>> result;
    int i = m, j = n;
    while (i > 0 || j > 0)
    {
        if (i > 0 && j > 0 && a[i-1] == b[j-1])
        {
            result.push_back({' ', a[i-1]});
            i--; j--;
        }
        else if (j > 0 && (i == 0 || dp[i][j-1] >= dp[i-1][j]))
        {
            result.push_back({'+', b[j-1]});
            j--;
        }
        else
        {
            result.push_back({'-', a[i-1]});
            i--;
        }
    }

    std::reverse(result.begin(), result.end());

    bool has_changes = false;
    for (auto& [c, l] : result) if (c != ' ') { has_changes = true; break; }
    if (!has_changes) return;

    printf("\033[1;94m--- %s\033[0m\n", filename.c_str());

    for (auto& [c, l] : result)
    {
        if (c == '+') printf("\033[1;92m+ %s\033[0m\n", l.c_str());
        else if (c == '-') printf("\033[1;91m- %s\033[0m\n", l.c_str());
    }

    printf("\n");
}

static std::vector<std::string> read_file_list(const std::string& path)
{
    std::vector<std::string> files;
    std::ifstream f(path);
    std::string line;
    while (std::getline(f, line))
        if (!line.empty()) files.push_back(line);
    return files;
}

void bull::Action::diff(const std::string& hash1, const std::string& hash2)
{
    std::string lang = bull::getCurrentLang();
    if (!bull::is_commit(hash1) || !bull::is_commit(hash2))
    {
        if (lang == "ru") log_.ERROR("Один или оба коммита не найдены!");
        else log_.ERROR("One or both commits not found!");
        return;
    }

    std::string cur_branch = bull::current_branch();
    std::string base1 = bull::init_dir + "/" + cur_branch + "/" + hash1 + "/";
    std::string base2 = bull::init_dir + "/" + cur_branch + "/" + hash2 + "/";

    auto files1 = read_file_list(base1 + bull::file_list);
    auto files2 = read_file_list(base2 + bull::file_list);

    for (const auto& f : files2)
    {
        bool in_old = std::find(files1.begin(), files1.end(), f) != files1.end();
        std::string p1 = in_old ? base1 + f : "";
        std::string p2 = base2 + f;

        if (!bull::isBinaryFile(p2))
        {
            if (!in_old)
                printf("\033[1;92m+++ %s (new file)\033[0m\n\n", f.c_str());
            else
                diff_files(p1, p2, f);
        }
    }

    for (const auto& f : files1)
    {
        bool in_new = std::find(files2.begin(), files2.end(), f) != files2.end();
        if (!in_new)
            printf("\033[1;91m--- %s (deleted)\033[0m\n\n", f.c_str());
    }
}

void bull::Action::diff(const std::string& hash)
{
    std::string lang = bull::getCurrentLang();
    if (!bull::is_commit(hash))
    {
        if (lang == "ru") log_.ERROR_NE("Коммит с хэшем '%s' не найден!", hash.c_str());
        else log_.ERROR_NE("No commits found with the hash '%s'", hash.c_str());
        return;
    }

    std::string cur_branch = bull::current_branch();
    std::string commit_base = bull::init_dir + "/" + cur_branch + "/" + hash + "/";
    auto committed = read_file_list(commit_base + bull::file_list);

    for (const auto& f : committed)
    {
        std::string committed_path = commit_base + f;
        std::string working_path = "./" + f;

        if (bull::isBinaryFile(committed_path)) continue;

        if (!std::filesystem::exists(working_path))
        {
            printf("\033[1;91m--- %s (deleted)\033[0m\n\n", f.c_str());
            continue;
        }

        diff_files(committed_path, working_path, f);
    }
}

void bull::Action::diff()
{
    std::string hash = bull::getLastCommit();
    std::string lang = bull::getCurrentLang();

    if (hash.empty())
    {
        if (lang == "ru") log_.ERROR("У вас нет коммитов!");
        else log_.ERROR("You don't have any commits!");
        return;
    }

    diff(hash);
}

void bull::Action::stash()
{
    std::string lang = bull::getCurrentLang();

    if (!bull::isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string staging_path = bull::init_dir + "/" + bull::data_list;
    std::string stash_path   = bull::init_dir + "/" + bull::stash_dir + "/";
    std::string stash_list   = stash_path + bull::file_list;

    std::ifstream check_staging(staging_path);
    bool staging_empty = true;
    std::string tmp;
    while (std::getline(check_staging, tmp))
        if (!tmp.empty()) { staging_empty = false; break; }
    check_staging.close();

    if (staging_empty)
    {
        if (lang == "ru") log_.WARNING("Нет файлов для сохранения в stash.");
        else log_.WARNING("Nothing to stash.");
        return;
    }

    std::filesystem::create_directories(stash_path);

    std::ifstream read_staging(staging_path);
    std::ofstream write_stash_list(stash_list);
    std::ifstream red;
    std::ofstream wrt;
    std::string line;

    while (std::getline(read_staging, line))
    {
        if (line.empty() || !std::filesystem::exists(line)) continue;

        std::string normalized = line;
        if (normalized.size() >= 2 && normalized[0] == '.' && normalized[1] == '/')
            normalized = normalized.substr(2);

        std::string target = stash_path + normalized;
        std::filesystem::create_directories(std::filesystem::path(target).parent_path());

        red.open(line, std::ios::binary);
        wrt.open(target, std::ios::binary);
        if (red.is_open() && wrt.is_open()) wrt << red.rdbuf();
        red.close();
        wrt.close();

        write_stash_list << normalized << "\n";
    }

    read_staging.close();
    write_stash_list.close();

    std::ofstream clear_staging(staging_path);
    clear_staging.close();

    std::string last = bull::getLastCommit();
    if (!last.empty()) unpack(last);

    if (lang == "ru") log_.INFO("Изменения сохранены в stash.");
    else log_.INFO("Changes saved to stash.");
}

void bull::Action::stash_pop()
{
    std::string lang = bull::getCurrentLang();

    if (!bull::isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string stash_path = bull::init_dir + "/" + bull::stash_dir + "/";
    std::string stash_list = stash_path + bull::file_list;

    if (!std::filesystem::exists(stash_list))
    {
        if (lang == "ru") log_.WARNING("Stash пуст.");
        else log_.WARNING("Stash is empty.");
        return;
    }

    std::ifstream read_list(stash_list);
    std::ofstream write_staging(bull::init_dir + "/" + bull::data_list);
    std::ifstream red;
    std::ofstream wrt;
    std::string line;

    while (std::getline(read_list, line))
    {
        if (line.empty()) continue;

        std::string src = stash_path + line;
        if (!std::filesystem::exists(src)) continue;

        std::filesystem::create_directories(std::filesystem::path(line).parent_path());

        red.open(src, std::ios::binary);
        wrt.open(line, std::ios::binary);
        if (red.is_open() && wrt.is_open()) wrt << red.rdbuf();
        red.close();
        wrt.close();

        write_staging << "./" << line << "\n";
    }

    read_list.close();
    write_staging.close();

    std::filesystem::remove_all(stash_path);

    if (lang == "ru") log_.INFO("Изменения восстановлены из stash.");
    else log_.INFO("Changes restored from stash.");
}
