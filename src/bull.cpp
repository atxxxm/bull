#include "../include/bull.hpp"
#include "../include/slog.hpp"
#include <filesystem>
#include <fstream>
#include <random>
#include <iostream>
#include <iterator>
#include <cctype>

slog::LOG log_;

bool bull::isBinaryFile(const std::string& filename) 
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) false;

    char byte;
    while (file.get(byte)) 
    {
        if (!std::isprint(byte) && !std::isspace(byte) && byte != '\n' && byte != '\t' && byte != '\r')  return true;
    }

    return false;
}

std::string bull::current_branch()
{
    std::string path = bull::init_dir + "/" + bull::config;
    std::ifstream stat_file(path);

    if (!stat_file.is_open()) return "";

    std::string line;

    std::getline(stat_file, line);
    std::getline(stat_file, line);

    stat_file.close();

    return line;
}

bool bull::is_branch(const std::string& branch)
{
    std::string line, path_to_head;
    path_to_head = bull::init_dir + "/" + bull::branch_list;

    std::ifstream read_head(path_to_head);

    while (std::getline(read_head, line))
    {
        if (line == branch)
        {
            return true;
        }
    }
    
    return false;
}

bool bull::is_commit(const std::string& hash)
{
    std::string cur_branch = current_branch();
    std::string path = bull::init_dir + "/" + cur_branch + "/" + bull::commit_list;
    std::string word;

    std::ifstream read_commit_list(path);

    while (read_commit_list >> word)
    {
        if (word == hash) return true;
    }
    
    read_commit_list.close();

    return false;
}

int bull::getLineCommit(const std::string& hash, const std::string& commit_name)
{   
    std::string path = bull::init_dir + "/" + hash + "/" + bull::commit_list, line;

    std::ifstream read_commit_list(path);

    int i = 0;
    size_t pos = 0;
    std::string fmt_line;

    while (std::getline(read_commit_list, line))
    {
        ++i;
        pos = line.find(' ');

        if (pos != std::string::npos)
        {
            fmt_line = line.substr(0, pos);

            if (fmt_line ==  commit_name)
            {
                break;
            }
        }
    }

    return i;
}

std::string bull::getLastCommit()
{
    std::string cur_branch, path, hash, line;

    cur_branch = current_branch();
    path = bull::init_dir + "/" + cur_branch + "/" + bull::commit_list;

    std::ifstream read_commit_list(path);

    std::getline(read_commit_list, line);

    read_commit_list.close();

    size_t pos = line.find(' ');
    hash = line.substr(0, pos);

    return hash;
}

// INIT

void bull::_init_::init() 
{
    std::string fmt_path = bull::init_dir + "/" + bull::base_branch;
    
    if (std::filesystem::create_directories(fmt_path))
    {
        log_.INFO("The project has been successfully initialized!");
    }
    else
    {
        log_.ERROR("The project has not been initialized!");
    }

    fmt_path = fmt_path + "/" + bull::commit_list;
    std::ofstream head(fmt_path);
    head.close();

    fmt_path = bull::init_dir + "/" + bull::branch_list;
    head.open(fmt_path);
    head << bull::base_branch << "\n";
    head.close(); 

    fmt_path = bull::init_dir + "/" + bull::config;
    head.open(fmt_path);
    head << "[CURRENT BRANCH]\n" << bull::base_branch;
    head.close();

    fmt_path = bull::init_dir + "/" + bull::lang_config;
    head.open(fmt_path);
    head << "en";
    head.close();
}

std::string bull::getCurrentLang()
{
    std::string lang;
    std::ifstream read_lang(bull::init_dir + "/" + bull::lang_config);

    std::getline(read_lang, lang);

    return lang;
}

void bull::_init_::changeLang(const std::string& lang)
{
    std::string current_lang = bull::getCurrentLang();
    if (lang == "ru" || lang == "RU" || lang == "Ru" || lang == "rU")
    {
        std::ofstream ch_lang(bull::init_dir + "/" + bull::lang_config);
        ch_lang << "ru";
        ch_lang.close();

        log_.CUSTOM("blue", "LANG", "ru");
    }
    else if (lang == "en" || lang == "EN" || lang == "En" || lang == "eN")
    {
        std::ofstream ch_lang(bull::init_dir + "/" + bull::lang_config);
        ch_lang << "en";
        ch_lang.close();

        log_.CUSTOM("blue", "LANG", "en");
    }
}

bool bull::_init_::isInitDir()
{
    return std::filesystem::is_directory(bull::init_dir);
}

void bull::_init_::ignore()
{
    std::string lang = bull::getCurrentLang();
    std::ofstream gnore(bull::bullgnore);
    gnore.close();
    log_.CUSTOM_NSL("purple", "+ .bullgnore");
}

void bull::_init_::collect_ignore()
{
    std::ifstream check_ignore(bullgnore);

    if (!check_ignore.is_open()) return;

    std::string line;
    
    while (std::getline(check_ignore, line))
    {
        if (line.empty()) continue;
        ignore_list.push_back(line);
    }

    check_ignore.close();
}

void bull::_init_::checkEdit()
{
    std::string lang = bull::getCurrentLang();
    std::string path, cur_branch, commit, added_files, modified_files, deleted_files, line;

    cur_branch = current_branch();
    commit = getLastCommit();
    if (!is_commit(commit) && commit.empty())
    {
        printf("\n");
        if (lang == "ru") log_.WARNING("У вас нет коммитов.");
        else log_.WARNING("You don't have any commits.");
        return;
    }

    path = bull::init_dir + "/" + cur_branch + "/" + commit + "/" + bull::file_list;

    std::vector<std::string> commited_files;
    std::vector<std::string> current_files;

    collect_ignore();
    ignore_list.push_back(bull::init_dir);

    std::ifstream read_file_list(path);

    while (std::getline(read_file_list, line))
    {
        if (line.empty()) continue;
        commited_files.push_back(line);
    }

    read_file_list.close();

    for (const auto& entry : std::filesystem::recursive_directory_iterator("."))
    { 
        if (!entry.is_regular_file()) continue;
        std::string entry_path = entry.path().string().substr(2);
        bool should_ignore = false;

        for (const auto& il : ignore_list)
        {
            if (entry_path.find(il) != std::string::npos)
            {
                should_ignore = true;
                break;
            }
        }

        if (!should_ignore) current_files.push_back(entry_path);
    }

    for (const auto& cur_file : current_files)
    {
        bool found = false;
        for (const auto& com_file : commited_files)
        {
            if (cur_file == com_file)
            {
                found = true;
                std::ifstream comm_file_stream(bull::init_dir + "/" + cur_branch + "/" + commit + "/" + com_file, std::ios::binary);
                std::ifstream cur_file_stream("./" + cur_file, std::ios::binary);

                if (!comm_file_stream.is_open() || !cur_file_stream.is_open()) continue;

                std::stringstream comm_buffer, cur_buffer;
                comm_buffer << comm_file_stream.rdbuf();
                cur_buffer << cur_file_stream.rdbuf();

                if (comm_buffer.str() != cur_buffer.str()) modified_files += "* " + cur_file + "\n";

                comm_file_stream.close();
                cur_file_stream.close();
                break;
            }
        }

        if (!found)
        {
            added_files += "+ " + cur_file + "\n";
        }
    }

    for (const auto& com_file : commited_files)
    {
        bool found = false;
        for (const auto& cur_file : current_files)
        {
            if (com_file == cur_file)
            {
                found = true;
                break;
            }
        }

        if (!found) deleted_files += "- " + com_file + "\n";
    }

    if (!added_files.empty())
    {
        printf("\n");
        if (lang == "ru") log_.CUSTOM("green", "ДОБАВЛЕННЫЕ ФАЙЛЫ", "");
        else log_.CUSTOM("green", "ADDED FILES", "");
        log_.CUSTOM_NSL("green", added_files);
    }

    if (!modified_files.empty())
    {
        printf("\n");
        if (lang == "ru") log_.CUSTOM("orange", "ИЗМЕНЁННЫЕ ФАЙЛЫ", "");
        else log_.CUSTOM("orange", "MODIFIED FILES", "");
        log_.CUSTOM_NSL("orange", modified_files);
    }

    if (!deleted_files.empty())
    {
        printf("\n");
        if (lang == "ru") log_.CUSTOM("red", "УДАЛЁННЫЕ ФАЙЛЫ", "");
        else log_.CUSTOM("red", "DELETED FILES", "");
        log_.CUSTOM_NSL("red", deleted_files);
    }

    if (added_files.empty() && modified_files.empty() && deleted_files.empty())
    {
        printf("\n");
        if (lang == "ru") log_.INFO("В рабочей директории нет изменений.");
        else log_.INFO("There are no changes in the working directory.");
    }

    ignore_list.clear();
    commited_files.clear();
    current_files.clear();
}

void bull::_init_::add()
{
    std::string lang = bull::getCurrentLang();
    if (!isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string file_l;
    collect_ignore();
    ignore_list.push_back(bull::init_dir);
     
    std::string path = bull::init_dir + "/" + bull::data_list;
    std::ofstream dataF(path);
    
    for (const auto& entry : std::filesystem::recursive_directory_iterator("."))
    {
        if (!entry.is_regular_file()) continue;
        bool should_ignore = false;
        std::string entry_path = entry.path().string();

        if (!ignore_list.empty())
        {
            for (const auto& il : ignore_list)
            {
                if (entry_path.find(il) != std::string::npos)
                {
                    should_ignore = true;
                    break;
                }
            }
        }

        if (!should_ignore)
        {
            dataF << entry_path << "\n";
            file_l += "+ " + entry_path + "\n";
        }
    }

    dataF.close();
    ignore_list.clear();

    log_.CUSTOM_NSL("green", file_l);
}

void bull::_init_::branch(const std::string& name)
{
    std::string lang = bull::getCurrentLang();
    if (!isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string path = bull::init_dir + "/" + name, result;
    
    if (std::filesystem::create_directory(path))
    {
        result = "-> + " + name;
        log_.CUSTOM_NSL("green", result);
    }
    else
    {
        if (lang == "ru") log_.ERROR_NE("Ветка '%s' не была успешно создана!", name.c_str());
        else log_.ERROR_NE("The branch '%s' was not created successfully!", name.c_str());
        return;
    }

    path = bull::init_dir + "/" + bull::branch_list;
    std::ofstream new_branch(path, std::ios::app);
    new_branch << name << "\n";
    new_branch.close();

    path = bull::init_dir + "/" + name + "/" + bull::commit_list;
    new_branch.open(path);
    new_branch.close();
}

void bull::_init_::list_branch()
{
    std::string lang = bull::getCurrentLang();
    if (!isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string cur_branch, res, path, line;
    cur_branch = current_branch();
    path = bull::init_dir + "/" + bull::branch_list;

    std::ifstream read_l_s(path);

    while (std::getline(read_l_s, line))
    {
        if (line == cur_branch)
        {
            res = "-> " + line;
            log_.CUSTOM_NSL("green", res);
            continue;
        }

        printf("%s\n", line.c_str());
    }
    
    read_l_s.close();
}

void bull::_init_::status()
{
    std::string lang = bull::getCurrentLang();
    if (!isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string line = bull::current_branch();
    if (lang == "ru")
    {
        log_.CUSTOM("light_blue", "ТЕКУЩАЯ ВЕТКА", line);
        log_.CUSTOM("blue", "ЯЗЫК", lang);
    }
    else
    {
        log_.CUSTOM("light_blue", "CURRENT branch", line);
        log_.CUSTOM("blue", "LANG", lang);
    }

    checkEdit();
}

// ACTION

bool bull::_action_::isInitDir()
{
    return std::filesystem::is_directory(bull::init_dir);
}

std::string bull::_action_::hash_gen()
{
    std::string hash;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9);
    std::uniform_int_distribution<> ch('a', 'z');

    for (int i = 0; i < 5; i++)
    {
        hash += std::to_string(dist(gen));
        hash += static_cast<char>(ch(gen));
    }

    return hash;
}

bool bull::_action_::isMoreOne()
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

std::string bull::_action_::select_random_branch(const std::string& exclude)
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

void bull::_action_::pack(const std::string& comm)
{
    std::string lang = bull::getCurrentLang();
    if (!isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string fmt_commit, commit, commit_list, lemma, line_comm;
    if (comm.size() > 4)
    {
        fmt_commit = comm.substr(comm.size() - 4, comm.size());
        
        if (fmt_commit == ".txt")
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
    }
    else commit = comm;

    std::string hash = hash_gen();
    std::string cur_branch = bull::current_branch();
    std::string path = bull::init_dir + "/" + cur_branch + "/" + bull::commit_list;

    std::ifstream read_commit_list(path);

    while (std::getline(read_commit_list, lemma))
    {
        if (lemma.empty()) continue;
        commit_list += lemma + "\n";
    }

    read_commit_list.close();
    
    std::ofstream tocommit(path);

    tocommit << hash << " " << commit << "\n" << commit_list;

    tocommit.close();

    path = bull::init_dir + "/" + cur_branch + "/" + hash;

    std::filesystem::create_directory(path);

    path = bull::init_dir + "/" + bull::data_list;

    std::string path_to_copy = bull::init_dir + "/" + cur_branch + "/" + hash + "/";

    std::string line, data;
    std::ifstream get_list_file(path);
    std::ofstream wrt;
    std::ifstream red;
    std::ofstream list_f(path_to_copy + bull::file_list);

    while (std::getline(get_list_file, line))
    {
        if (line.empty()) continue;

        if (!std::filesystem::exists(line)) continue;

        std::string f_l = line;

        list_f << f_l.substr(2) << "\n";

        std::string target_file = path_to_copy + line.substr(2);
        std::string target_dir = std::filesystem::path(target_file).parent_path().string();

        if (!target_dir.empty()) 
        {
            std::filesystem::create_directories(target_dir);
        }

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

    std::string result = "+ " + cur_branch + "->" + hash + " - " + commit;
    log_.CUSTOM_NSL("green", result);
}

void bull::_action_::unpack(const std::string& hash)
{
    std::string lang = bull::getCurrentLang();
    if (!is_commit(hash))
    {
        if (lang == "ru") log_.ERROR_NE("Коммит с хэшем '%s' не существует!", hash.c_str());
        else log_.ERROR_NE("The commit with the hash '%s' does not exist!", hash.c_str());
        return;
    }

    std::string line, path, cur_branch, path_to_read, fmt_dir; 
    cur_branch = bull::current_branch();
    path = bull::init_dir + "/" + cur_branch + "/" + hash + "/" + bull::file_list;
    path_to_read = bull::init_dir + "/" + cur_branch + "/" + hash + "/";

    std::ifstream r_file_list(path);
    std::ifstream red;
    std::ofstream wrt;

    while (std::getline(r_file_list, line))
    {
        if (line.empty()) continue;

        if (!std::filesystem::exists(path_to_read + line)) continue;

        fmt_dir = std::filesystem::path(line).parent_path().string();

        if (!fmt_dir.empty())
        {
            std::filesystem::create_directories(fmt_dir);
        }

        red.open(path_to_read + line, std::ios::binary);
        wrt.open(line, std::ios::binary);

        if (red.is_open() && wrt.is_open())
        {
            wrt << red.rdbuf();
        }

        red.close();
        wrt.close();
    }

    r_file_list.close();

    if (lang == "ru") log_.INFO_NE("Коммит '%s' успешно распакован", hash.c_str());
    else log_.INFO_NE("The commit '%s' has been successfully unpacked", hash.c_str());
}

void bull::_action_::set(const std::string& branch_name)
{
    std::string lang = bull::getCurrentLang();
    if (!is_branch(branch_name))
    {
        if (lang == "ru") log_.ERROR_NE("Ветка '%s' не существует!", branch_name.c_str());
        else log_.ERROR_NE("Branch '%s' does not exist!", branch_name.c_str());
        std::string fmt = lang == "ru" ? "Хотите создать ветку '" + branch_name + "'? (Y/N): " : "Do you want to create a '" + branch_name + "'? (Y/N): ";
        log_.CUSTOM_NSL_WITHOUT_N("purple", fmt);

        char answer;
        std::cin >> answer;

        if (answer == 'Y' || answer == 'y')
        {
            bull::_init_ init;
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

void bull::_action_::remove_branch(const std::string& branch_name)
{
    std::string lang = bull::getCurrentLang();
    std::string cur_branch = bull::current_branch();

    if (!is_branch(branch_name))
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

        std::string fmt = lang == "ru" ? "Переключиться автоматически на другую ветку? (Y/N): " : "Will it switch to another branch automatically? (Y/N): ";
        log_.CUSTOM_NSL_WITHOUT_N("purple", fmt);
        char answer;
        std::cin >> answer;

        if (answer == 'Y' || answer == 'y')
        {
            std::string random_branch = select_random_branch(branch_name);
            set(random_branch);
        }
        else return;
    }

    std::string path, line, new_branch_list, res;

    path = bull::init_dir + "/" + bull::branch_list; 

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

    path = bull::init_dir + "/" + branch_name;

    std::filesystem::remove_all(path);

    res = "-> - " + branch_name;
    log_.CUSTOM_NSL("red", res);
}

void bull::_action_::remove_commit(const std::string& commit_hash)
{
    std::string lang = bull::getCurrentLang();
    std::string path, line, cur_branch, new_commit_list, res;

    cur_branch = current_branch();

    path = bull::init_dir + "/" + cur_branch + "/" + bull::commit_list;

    if (!is_commit(commit_hash))
    {
        if (lang == "ru") log_.ERROR_NE("Коммит с хэшем '%s' не найден!", commit_hash.c_str());
        else log_.ERROR_NE("No commits found with the hash '%s'", commit_hash.c_str());
        return;
    }

    std::ifstream read_commit_list(path);

    int del_line = bull::getLineCommit(cur_branch, commit_hash), i = 0;

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

    path = bull::init_dir + "/" + cur_branch + "/" + commit_hash;

    std::filesystem::remove_all(path);

    res = "- " + cur_branch + "->" + commit_hash;
    log_.CUSTOM_NSL("red", res);
}

void bull::_action_::log()
{
    std::string lang = bull::getCurrentLang();
    if (!isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string branch, line, path_to_branch, res;
    branch = bull::current_branch();

    path_to_branch = bull::init_dir + "/" + branch + "/" + bull::commit_list;

    std::ifstream read_commites(path_to_branch);

    if (!read_commites.is_open()) return;


    if (lang == "ru")
    {
        res = "ветка -> " + branch + "\n";
        log_.CUSTOM_NSL("green", res);
    }
    else
    {
        res = "branch -> " + branch + "\n";
        log_.CUSTOM_NSL("green", res);
    }

    std::getline(read_commites, line);

    log_.CUSTOM_NSL("orange", line);

    while (std::getline(read_commites, line))
    {
        printf("%s\n", line.c_str());
    }
    
    read_commites.close();
}

void bull::_action_::reset()
{
    std::string lang = bull::getCurrentLang();
    if (!isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }
    
    std::string line, hash;

    line = bull::getLastCommit();

    if (line.empty())
    {
        if (lang == "ru") log_.ERROR("У вас нет коммитов!");
        else log_.ERROR("You don't have any commits!");
        return;
    }

    size_t pos = line.find(' ');
    hash = line.substr(0, pos);

    unpack(hash);
}

void bull::_action_::show_func(const std::string& commit_hash)
{
    std::string lang = getCurrentLang();
    if (!is_commit(commit_hash))
    {
        if (lang == "ru") log_.ERROR_NE("Коммит с хэшем '%s' не найден!", commit_hash.c_str());
        else log_.ERROR_NE("No commits found with the hash '%s'", commit_hash.c_str());
        return;
    }

    std::string path_to_file_list, path_to_commit, cur_branch, line, res;
    
    cur_branch = current_branch();
    path_to_file_list = bull::init_dir + "/" + cur_branch + "/" + commit_hash + "/" + bull::file_list;
    path_to_commit = bull::init_dir + "/" + cur_branch + "/" + commit_hash + "/";

    std::ifstream read_to_file_list(path_to_file_list);
    std::ifstream read_file;

    res = cur_branch + "->" + commit_hash;
    log_.CUSTOM_NSL("green", res);

    while (std::getline(read_to_file_list, line))
    {
        if (line.empty()) continue;

        if (isBinaryFile(path_to_commit + line)) continue;

        read_file.open(path_to_commit + line, std::ios::binary);

        log_.CUSTOM_NSL("blue", line + ":");
        std::cout << std::string(std::istreambuf_iterator<char>(read_file), std::istreambuf_iterator<char>()) << std::endl;
        printf("\n");

        read_file.close();
    }
    
    read_to_file_list.close();
}

void bull::_action_::show(const std::string& commit_hash)
{
    if (commit_hash.empty()) return;
    show_func(commit_hash);
}

void bull::_action_::show_last()
{
    std::string commit = getLastCommit();
    
    if (commit.empty()) return;

    show_func(commit);
}

void bull::_action_::show_cur(const std::string& filename)
{
    std::string path, cur_branch, commit, lang, res, line;
    lang = getCurrentLang();
    cur_branch = current_branch();
    commit = getLastCommit();
    path = bull::init_dir + "/" + cur_branch + "/" + commit + "/" + filename;

    std::ifstream read_file(path, std::ios::binary);

    if (!read_file.is_open())
    {
        res = cur_branch + "->" + commit;
        if (lang == "ru") log_.ERROR_NE("Файл '%s' в %s не найден!", filename.c_str(), res.c_str());
        else log_.ERROR_NE("The file '%s' was not found in the %s!", filename.c_str(), res.c_str());
        return;
    }

    log_.CUSTOM_NSL("blue", filename + ":");

    while (std::getline(read_file, line))
    {
        printf("%s\n", line.c_str());
    }

    read_file.close();
}

void bull::_action_::comm_list_func(const std::string& commit_hash)
{
    std::string lang = getCurrentLang();
    if (!is_commit(commit_hash))
    {
        if (lang == "ru") log_.ERROR_NE("Коммит с хэшем '%s' не найден!", commit_hash.c_str());
        else log_.ERROR_NE("No commits found with the hash '%s'", commit_hash.c_str());
        return;
    }

    std::string path, cur_branch, line, res;
    cur_branch = current_branch();
    path = bull::init_dir + "/" + cur_branch + "/" + commit_hash + + "/" + bull::file_list;

    std::ifstream read_fl(path);

    res = cur_branch + "->" + commit_hash + "\n";
    log_.CUSTOM_NSL("green", res);

    if (lang == "ru") log_.CUSTOM("blue", "ФАЙЛЫ", "");
    else log_.CUSTOM("blue", "FILES", "");

    while (std::getline(read_fl, line))
    {
        printf("%s\n", line.c_str());
    }

    read_fl.close();
}

void bull::_action_::comm_list(const std::string& commit_hash)
{
    comm_list_func(commit_hash);
}

void bull::_action_::comm_list_last()
{
    std::string comm = getLastCommit();
    comm_list_func(comm);
}
