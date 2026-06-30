#include "../include/bull.hpp"
#include <functional>
#include <map>
#include <string>
#include <cstdio>

int main(int argc, char *argv[])
{
    auto print_help = [](){
        printf("\nUsage: bull <command> [args]\n\n");
        printf("Repository:\n");
        printf("  init                   Initialize a new repository\n");
        printf("  status                 Show current branch and working tree changes\n");
        printf("  log                    Show commit history\n");
        printf("\nStaging:\n");
        printf("  add .                  Stage all files\n");
        printf("  add <file>...          Stage specific files\n");
        printf("  add -clean             Clear staging area\n");
        printf("  gnore                  Create .bullgnore file\n");
        printf("\nCommits:\n");
        printf("  pack <message>         Create a commit with staged files\n");
        printf("  pack <file.txt>        Create a commit using message from .txt file\n");
        printf("  unpack <hash>          Restore files from a commit\n");
        printf("  reset                  Restore files from the latest commit\n");
        printf("  remove -c <hash>       Delete a commit\n");
        printf("\nBranches:\n");
        printf("  new <name>             Create a new branch\n");
        printf("  branch                 List all branches\n");
        printf("  set <name>             Switch to a branch\n");
        printf("  remove -b <name>       Delete a branch\n");
        printf("\nInspect:\n");
        printf("  show <hash>            Show contents of a commit\n");
        printf("  show -last             Show contents of the latest commit\n");
        printf("  show -cur <file>       Show a file from the latest commit\n");
        printf("  list <hash>            List files in a commit\n");
        printf("  list -last             List files in the latest commit\n");
        printf("  diff                   Diff working tree vs latest commit\n");
        printf("  diff <hash>            Diff working tree vs a commit\n");
        printf("  diff <hash1> <hash2>   Diff between two commits\n");
        printf("Stash:\n");
        printf("  stash                  Save staged changes and restore last commit\n");
        printf("  stash pop              Restore saved changes from stash\n");
        printf("\nSettings:\n");
        printf("  lang <ru|en>           Change output language\n");
        printf("  help                   Show this help message\n\n");
    };

    if (argc < 2)
    {
        print_help();
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
        {"stash pop",   [&]{ action.stash_pop(); }},
        {"stash",       [&]{ action.stash(); }},
        {"help",        [&]{ print_help(); }},
    };

    std::string key = sub.empty() ? cmd : cmd + " " + sub;

    auto it = commands.find(key);
    if (it == commands.end() && !sub.empty())
        it = commands.find(cmd);

    if (it != commands.end())
        it->second();
    else
        printf("Unknown command: '%s'. Run 'bull help' to see available commands.\n", cmd.c_str());

    return 0;
}
