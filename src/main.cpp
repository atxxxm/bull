#include "../include/bull.hpp"
#include <functional>
#include <map>
#include <string>
#include <cstdio>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: bull <command> [args]\n");
        return 1;
    }

    bull::Init init;
    bull::Action action;

    std::string cmd = argv[1];
    std::string sub = argc > 2 ? argv[2] : "";
    std::string arg3 = argc > 3 ? argv[3] : "";

    std::map<std::string, std::function<void()>> commands = {
        {"init",        [&]{ init.init(); }},
        {"add -clean",  [&]{ init.add_clean(); }},
        {"add",         [&]{ if (!sub.empty()) init.add(2, argc, argv); }},
        {"gnore",       [&]{ init.ignore(); }},
        {"new",         [&]{ if (!sub.empty()) init.branch(sub); }},
        {"branch",      [&]{ init.list_branch(); }},
        {"status",      [&]{ init.status(); }},
        {"lang",        [&]{ if (!sub.empty()) init.changeLang(sub); }},
        {"pack",        [&]{ if (!sub.empty()) action.pack(sub); }},
        {"unpack",      [&]{ if (!sub.empty()) action.unpack(sub); }},
        {"log",         [&]{ action.log(); }},
        {"set",         [&]{ if (!sub.empty()) action.set(sub); }},
        {"remove -b",   [&]{ if (!arg3.empty()) action.remove_branch(arg3); }},
        {"remove -c",   [&]{ if (!arg3.empty()) action.remove_commit(arg3); }},
        {"reset",       [&]{ action.reset(); }},
        {"show -last",  [&]{ action.show_last(); }},
        {"show -cur",   [&]{ if (!arg3.empty()) action.show_cur(arg3); }},
        {"show",        [&]{ if (!sub.empty()) action.show(sub); }},
        {"list -last",  [&]{ action.comm_list_last(); }},
        {"list",        [&]{ if (!sub.empty()) action.comm_list(sub); }},
        {"diff",        [&]{
            if (!sub.empty() && !arg3.empty()) action.diff(sub, arg3);
            else if (!sub.empty()) action.diff(sub);
            else action.diff();
        }},
    };

    std::string key = sub.empty() ? cmd : cmd + " " + sub;

    auto it = commands.find(key);
    if (it == commands.end() && !sub.empty())
        it = commands.find(cmd);

    if (it != commands.end())
        it->second();
    else
        printf("Unknown command: %s\n", cmd.c_str());

    return 0;
}
