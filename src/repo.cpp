#include "../include/bull.hpp"
#include "../include/slog.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>

extern slog::LOG log_;
extern std::string lang_cache;

void bull::Init::init()
{
    std::string init_path = bull::init_dir + "/";

    if (std::filesystem::create_directories(init_path + bull::base_branch)) log_.INFO("The project has been successfully initialized!");
    else log_.ERROR("The project has not been initialized!");

    std::ofstream head;

    head.open(init_path + bull::commit_list);
    head.close();

    head.open(init_path + bull::branch_list);
    head << bull::base_branch << "\n";
    head.close();

    head.open(init_path + bull::config);
    head << "[CURRENT BRANCH]\n" << bull::base_branch;
    head.close();

    head.open(init_path + bull::lang_config);
    head << "en";
    head.close();
}

void bull::Init::changeLang(const std::string& lang)
{
    std::string normalized;
    if (lang == "ru" || lang == "RU" || lang == "Ru" || lang == "rU") normalized = "ru";
    else if (lang == "en" || lang == "EN" || lang == "En" || lang == "eN") normalized = "en";
    else return;

    std::ofstream ch_lang(bull::init_dir + "/" + bull::lang_config);
    ch_lang << normalized;
    ch_lang.close();

    lang_cache = normalized;

    log_.CUSTOM("blue", "LANG", normalized);
}

void bull::Init::ignore()
{
    std::ofstream gnore(bull::bullgnore);
    gnore.close();
    log_.CUSTOM_NSL("purple", "+ .bullgnore");
}

std::vector<std::string> bull::Init::collect_ignore()
{
    std::vector<std::string> ignore_list;
    std::ifstream check_ignore(bull::bullgnore);

    if (!check_ignore.is_open()) return ignore_list;

    std::string line;
    while (std::getline(check_ignore, line))
    {
        if (line.empty()) continue;
        ignore_list.push_back(line);
    }

    return ignore_list;
}

void bull::Init::checkEdit(const std::vector<std::string>& ignore_list)
{
    std::string lang = bull::getCurrentLang();
    std::string path, cur_branch, commit, added_files, modified_files, deleted_files, line;

    cur_branch = bull::current_branch();
    commit = bull::getLastCommit();

    if (commit.empty() || !bull::is_commit(commit))
    {
        printf("\n");
        if (lang == "ru") log_.WARNING("У вас нет коммитов.");
        else log_.WARNING("You don't have any commits.");
        return;
    }

    path = bull::init_dir + "/" + cur_branch + "/" + commit + "/" + bull::file_list;

    std::vector<std::string> ign = ignore_list;
    ign.push_back(bull::init_dir);

    std::unordered_set<std::string> commited_set;

    std::ifstream read_file_list(path);
    while (std::getline(read_file_list, line))
    {
        if (!line.empty()) commited_set.insert(line);
    }
    read_file_list.close();

    std::unordered_set<std::string> current_set;

    for (const auto& entry : std::filesystem::recursive_directory_iterator("."))
    {
        if (!entry.is_regular_file()) continue;
        std::string entry_path = entry.path().string().substr(2);
        bool should_ignore = false;

        for (const auto& il : ign)
        {
            if (entry_path.find(il) != std::string::npos)
            {
                should_ignore = true;
                break;
            }
        }

        if (!should_ignore) current_set.insert(entry_path);
    }

    for (const auto& cur_file : current_set)
    {
        if (commited_set.count(cur_file))
        {
            std::ifstream comm_file_stream(bull::init_dir + "/" + cur_branch + "/" + commit + "/" + cur_file, std::ios::binary);
            std::ifstream cur_file_stream("./" + cur_file, std::ios::binary);

            if (!comm_file_stream.is_open() || !cur_file_stream.is_open()) continue;

            std::stringstream comm_buffer, cur_buffer;
            comm_buffer << comm_file_stream.rdbuf();
            cur_buffer << cur_file_stream.rdbuf();

            if (comm_buffer.str() != cur_buffer.str()) modified_files += "* " + cur_file + "\n";
        }
        else
        {
            added_files += "+ " + cur_file + "\n";
        }
    }

    for (const auto& com_file : commited_set)
    {
        if (!current_set.count(com_file)) deleted_files += "- " + com_file + "\n";
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

}

void bull::Init::add_clean()
{
    std::string lang = bull::getCurrentLang();
    if (!bull::isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string path = bull::init_dir + "/" + bull::data_list;
    std::ofstream cl(path);
    cl.close();
}

void bull::Init::add(int startIndex, int argc, char* argv[])
{
    std::string lang = bull::getCurrentLang();
    if (!bull::isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::vector<std::string> args = bull::getArguments(startIndex, argc, argv);
    std::string file_l, entry_path;

    std::vector<std::string> ignore_list = collect_ignore();
    ignore_list.push_back(bull::init_dir);
    std::string path = bull::init_dir + "/" + bull::data_list;

    if (args.size() == 1 && args[0] == ".")
    {
        std::ofstream dataF(path);

        for (const auto& entry : std::filesystem::recursive_directory_iterator("."))
        {
            if (!entry.is_regular_file()) continue;
            bool should_ignore = false;
            entry_path = entry.path().string();

            for (const auto& il : ignore_list)
            {
                if (entry_path.find(il) != std::string::npos)
                {
                    should_ignore = true;
                    break;
                }
            }

            if (!should_ignore)
            {
                dataF << entry_path << "\n";
                file_l += "+ " + entry_path + "\n";
            }
        }

        dataF.close();

        log_.CUSTOM_NSL("green", file_l);
        return;
    }

    std::vector<std::string> existing;
    std::string ex_line;
    std::ifstream read_existing(path);
    while (std::getline(read_existing, ex_line))
    {
        if (!ex_line.empty()) existing.push_back(ex_line);
    }
    read_existing.close();

    std::ofstream w_f(path);
    for (const auto& e : existing) w_f << e << "\n";

    for (const auto& a : args)
    {
        if (!std::filesystem::exists(a) || !std::filesystem::is_regular_file(a)) continue;

        entry_path = "./" + a;

        bool already = false;
        for (const auto& e : existing)
            if (e == entry_path) { already = true; break; }

        if (!already)
        {
            w_f << entry_path << "\n";
            file_l += "+ " + entry_path + "\n";
        }
    }

    w_f.close();

    log_.CUSTOM_NSL("green", file_l);
}

void bull::Init::branch(const std::string& name)
{
    std::string lang = bull::getCurrentLang();
    if (!bull::isValidName(name))
    {
        if (lang == "ru") log_.ERROR_NE("Недопустимое имя ветки: '%s'", name.c_str());
        else log_.ERROR_NE("Invalid branch name: '%s'", name.c_str());
        return;
    }
    if (!bull::isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string path = bull::init_dir + "/" + name;

    if (std::filesystem::create_directory(path))
    {
        log_.CUSTOM_NSL("green", "-> + " + name);
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

void bull::Init::list_branch()
{
    std::string lang = bull::getCurrentLang();

    if (!bull::isInitDir())
    {
        if (lang == "ru") log_.ERROR("Проект не был инициализирован!");
        else log_.ERROR("The project has not been initialized!");
        return;
    }

    std::string cur_branch = bull::current_branch();
    std::string path = bull::init_dir + "/" + bull::branch_list;

    std::ifstream read_l_s(path);
    std::string line;

    while (std::getline(read_l_s, line))
    {
        if (line == cur_branch)
        {
            log_.CUSTOM_NSL("green", "-> " + line);
            continue;
        }

        printf("%s\n", line.c_str());
    }

    read_l_s.close();
}

void bull::Init::status()
{
    std::string lang = bull::getCurrentLang();

    if (!bull::isInitDir())
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
        log_.CUSTOM("light_blue", "CURRENT BRANCH", line);
        log_.CUSTOM("blue", "LANG", lang);
    }

    checkEdit(collect_ignore());
}
