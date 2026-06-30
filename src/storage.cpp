#include "../include/bull.hpp"
#include "../include/slog.hpp"
#include <filesystem>
#include <fstream>
#include <cctype>
#include <sstream>

slog::LOG log_;
std::string lang_cache;

bool bull::isValidName(const std::string& name)
{
    if (name.empty()) return false;
    if (name.find("..") != std::string::npos) return false;
    if (name.find('/') != std::string::npos) return false;
    if (name.find('\\') != std::string::npos) return false;
    if (name.find('|') != std::string::npos) return false;
    return true;
}

bull::CommitInfo bull::parseCommitLine(const std::string& line)
{
    CommitInfo info;

    if (line.find('|') != std::string::npos)
    {
        std::istringstream ss(line);
        std::getline(ss, info.hash, '|');
        std::getline(ss, info.timestamp, '|');
        std::getline(ss, info.parent_hash, '|');
        std::getline(ss, info.message);
    }
    else
    {
        size_t pos = line.find(' ');
        info.hash = line.substr(0, pos);
        info.message = (pos != std::string::npos) ? line.substr(pos + 1) : "";
        info.timestamp = "0";
        info.parent_hash = "none";
    }

    return info;
}

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

    std::ifstream read_commit_list(path);
    std::string line;

    while (std::getline(read_commit_list, line))
    {
        if (line.empty()) continue;
        if (parseCommitLine(line).hash == hash) return true;
    }

    return false;
}

int bull::getLineCommit(const std::string& branch, const std::string& commit_hash)
{
    std::string path = bull::init_dir + "/" + branch + "/" + bull::commit_list;

    std::ifstream read_commit_list(path);
    std::string line;
    int i = 0;

    while (std::getline(read_commit_list, line))
    {
        ++i;
        if (!line.empty() && parseCommitLine(line).hash == commit_hash) break;
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

    if (line.empty()) return "";
    return parseCommitLine(line).hash;
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

std::string bull::resolve_file_path(const std::string& branch, const std::string& commit_hash, const std::string& filename)
{
    std::string direct = bull::init_dir + "/" + branch + "/" + commit_hash + "/" + filename;
    if (std::filesystem::exists(direct)) return direct;

    std::string ref_list_path = bull::init_dir + "/" + branch + "/" + commit_hash + "/" + bull::ref_list;
    std::ifstream rf(ref_list_path);
    std::string line;

    while (std::getline(rf, line))
    {
        size_t pos = line.find('|');
        if (pos == std::string::npos) continue;

        if (line.substr(0, pos) == filename)
        {
            std::string ref_hash = line.substr(pos + 1);
            return resolve_file_path(branch, ref_hash, filename);
        }
    }

    return "";
}

std::string bull::getCurrentLang()
{
    if (!lang_cache.empty()) return lang_cache;

    std::ifstream read_lang(bull::init_dir + "/" + bull::lang_config);
    std::getline(read_lang, lang_cache);

    return lang_cache;
}
