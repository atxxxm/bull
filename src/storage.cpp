#include "../include/bull.hpp"
#include "../include/slog.hpp"
#include <filesystem>
#include <fstream>
#include <cctype>

slog::LOG log_;
std::string lang_cache;

bool bull::isInitDir()
{
    return std::filesystem::is_directory(bull::init_dir);
}

bool bull::isBinaryFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    char byte;
    while (file.get(byte))
    {
        if (!std::isprint(byte) && !std::isspace(byte) && byte != '\n' && byte != '\t' && byte != '\r') return true;
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
    std::string line;
    std::string path_to_head = bull::init_dir + "/" + bull::branch_list;

    std::ifstream read_head(path_to_head);

    while (std::getline(read_head, line))
    {
        if (line == branch) return true;
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

int bull::getLineCommit(const std::string& branch, const std::string& commit_hash)
{
    std::string path = bull::init_dir + "/" + branch + "/" + bull::commit_list, line;

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

            if (fmt_line == commit_hash) break;
        }
    }

    return i;
}

std::string bull::getLastCommit()
{
    std::string cur_branch = current_branch();
    std::string path = bull::init_dir + "/" + cur_branch + "/" + bull::commit_list;

    std::ifstream read_commit_list(path);

    std::string line;
    std::getline(read_commit_list, line);

    read_commit_list.close();

    size_t pos = line.find(' ');
    return line.substr(0, pos);
}

std::vector<std::string> bull::getArguments(int startIndex, int argc, char* argv[])
{
    std::vector<std::string> args;

    for (int i = startIndex; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }

    return args;
}

std::string bull::getCurrentLang()
{
    if (!lang_cache.empty()) return lang_cache;

    std::ifstream read_lang(bull::init_dir + "/" + bull::lang_config);
    std::getline(read_lang, lang_cache);

    return lang_cache;
}
