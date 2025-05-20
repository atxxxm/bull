#include <iostream>
#include <linux/string.h>

#include "bull.hpp"

int main(int argc, char *argv[])
{
    LOG logger;
    _init_ init;
    _action_ action;

    if (strcmp(argv[1], "init") == 0)
    {
        init.create_folder_for_init();
    }
    else if (strcmp(argv[1], "pack") == 0)
    {
        if (argc < 3 || argv[2][0] == '\0')
        {
            logger.ERROR("The name of the box cannot be empty!");
            return -1;
        }
        init.add_project(argv[2]);
    }
    else if (strcmp(argv[1], "gnore") == 0)
    {
        init.gnore();
    }
    else if (strcmp(argv[1], "list") == 0)
    {
        action.get_box_list_full();
    }
    else if (strcmp(argv[1], "unpack") == 0 && strcmp(argv[2], "-id") == 0)
    {
        if (argc < 4 || argv[3][0] == '\0')
        {
            logger.ERROR("The ID of the box cannot be empty!");
            return -1;
        }
        action.set_box_id(argv[3]);
    }

    else if (strcmp(argv[1], "unpack") == 0 && strcmp(argv[2], "last") == 0)
    {
        action.set_last_box();
    }
    
    else if (strcmp(argv[1], "unpack") == 0)
    {
        if (argc < 3 || argv[2][0] == '\0')
        {
            logger.ERROR("The name of the box cannot be empty!");
            return -1;
        }
        action.set_box(argv[2]);
    }
    else if (strcmp(argv[1], "delete") == 0 && strcmp(argv[2], "-name") == 0)
    {
        if (argc < 4 || argv[3][0] == '\0')
        {
            logger.ERROR("The name of the box cannot be empty!");
            return -1;
        }
        action.delete_name(argv[3]);
    }
    else if (strcmp(argv[1], "delete") == 0 && strcmp(argv[2], "-id") == 0)
    {
        if (argc < 4 || argv[3][0] == '\0')
        {
            logger.ERROR("The ID of the box cannot be empty!");
            return -1;
        }
        action.delete_id(argv[3]);
    }
    else if (strcmp(argv[1], "delete") == 0 || strcmp(argv[1], "-last") == 0)
    {
        action.delete_last();
    }
    else if (strcmp(argv[1], "change") == 0 && strcmp(argv[2], "-dir") == 0)
    {
        action.dir_change();
    }
    else if (strcmp(argv[1], "change") == 0 && strcmp(argv[2], "-file") == 0)
    {
        action.file_change();
    }
    else if (strcmp(argv[1], "change") == 0 && strcmp(argv[2], "-data") == 0)
    {
        action.data_change();
    }
    else if (strcmp(argv[1], "change") == 0 && strcmp(argv[2], "-all") == 0)
    {
        action.all_change();
    }
    else
    {
        logger.ERROR("Unknown command");
    }

    return 0;
}
