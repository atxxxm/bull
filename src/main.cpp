#include "../include/bull.hpp"
#include <string.h>


int main(int argc, char const *argv[])
{
    bull::_init_ init;
    bull::_action_ action;
    
    if (strcmp(argv[1], "init") == 0) init.init();

    else if (strcmp(argv[1], "add") == 0) init.add();

    else if (strcmp(argv[1], "gnore") == 0) init.ignore();

    else if (strcmp(argv[1], "new") == 0) init.branch(std::string(argv[2]));

    else if (strcmp(argv[1], "branch") == 0) init.list_branch();

    else if (strcmp(argv[1], "status") == 0) init.status();

    else if (strcmp(argv[1], "lang") == 0) init.changeLang(std::string(argv[2]));

    else if (strcmp(argv[1], "pack") == 0) action.pack(std::string(argv[2]));

    else if (strcmp(argv[1], "unpack") == 0) action.unpack(std::string(argv[2]));

    else if (strcmp(argv[1], "log") == 0) action.log();

    else if (strcmp(argv[1], "set") == 0) action.set(std::string(argv[2]));

    else if (strcmp(argv[1], "remove") == 0 && strcmp(argv[2], "-b") == 0) action.remove_branch(std::string(argv[3]));

    else if (strcmp(argv[1], "remove") == 0 && strcmp(argv[2], "-c") == 0) action.remove_commit(std::string(argv[3]));

    else if (strcmp(argv[1], "reset") == 0) action.reset();
    
    return 0;
}
