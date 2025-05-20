#include "bull.hpp"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <linux/string.h>
#include <vector>
#include <unistd.h>
#include <random>
#include <sstream>
#include <fcntl.h>
#include <cstdint>
#include <unordered_set>
#include "slog.hpp"

//FILES_AND_FOLDERS
bool files_and_folders::is_bin(const std::string& file)
{
    int fd = open(file.c_str(), O_RDONLY);

    if (fd == -1) return false;

    uint8_t magic[4];

    if (read(fd, magic, sizeof(magic)) != sizeof(magic))
    {
        close(fd);
        return false;
    }

    close(fd);

    return (magic[0] == 0x7F && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F');
}

int files_and_folders::check_bullgnore()
{
    bullgnore.clear();
    std::ifstream bg(bullgnore_name);

    bullgnore.push_back(".bullgnore");

    if (!bg.is_open()) return -1;

    std::string line;

    while (std::getline(bg, line))
    {
        bullgnore.push_back(line);
    }

    bg.close();

    return 0;
}

int files_and_folders::dir_list(char *path)
{
    DIR *dir = opendir(path);

    if (dir == NULL) return -1;

    char fullpath[1024];
    struct dirent *d;

    while ((d = readdir(dir)) != NULL)
    {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, config_folder_name) == 0) continue;

        if (d->d_type == DT_DIR)
        {
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, d->d_name);
            vec_dir_path.push_back(fullpath);
            dir_list(fullpath);
        }
        
    }
    closedir(dir);

    return 0;
}

int files_and_folders::write_dir_list_in_file()
{
    if (vec_dir_path.empty()) return -1;
    
    if (!bullgnore.empty())
    {
        for (int i = 0; i < bullgnore.size(); i++)
        {
            for (int j = 0; j < vec_dir_path.size(); j++)
            {
                if (bullgnore[i] == vec_dir_path[j])
                {
                    auto it = vec_dir_path.begin() + j;
                    vec_dir_path.erase(it);
                }
            }
        }
    }

    if (vec_dir_path.empty()) return -1;

    std::ofstream file(folders_list_name);

    for (const auto& vec : vec_dir_path)
    {
        file << vec << "\n";
    }

    file.close();
    vec_dir_path.clear();


    return 0;
}

int files_and_folders::files_list_in_folder()
{
    std::string line, full;
    std::ifstream folder(folders_list_name);

    if (!folder.is_open()) return -1;

    DIR *dir;
    struct dirent *d;

    while (std::getline(folder, line))
    {
        dir = opendir(line.c_str());

        while ((d = readdir(dir)) != NULL)
        {
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || is_bin(d->d_name)) continue;

            if (d->d_type == DT_REG)
            {
                full = line + "/" + d->d_name;
                vec_file_in_folder_path.push_back(full);
            }
            
        }

        closedir(dir);
    }

    folder.close();

    return 0;
}

int files_and_folders::write_files_in_folder_list_in_file()
{
    if (vec_file_in_folder_path.empty()) return -1;

    if (!bullgnore.empty())
    {
        for (int i = 0; i < bullgnore.size(); i++)
        {
            for (int j = 0; j < vec_file_in_folder_path.size(); j++)
            {
                if (bullgnore[i] == vec_file_in_folder_path[j])
                {
                    auto it = vec_file_in_folder_path.begin() + j;
                    vec_file_in_folder_path.erase(it);
                }
            }
        }
    }

    if (vec_file_in_folder_path.empty()) return -1;

    std::ofstream file(files_list_name, std::ios::app);

    if (!file.is_open())
    {
        vec_file_in_folder_path.clear();
        return -1;
    }

    for (const auto& vec : vec_file_in_folder_path)
    {
        file << vec << "\n";
    }


    file.close();
    vec_file_in_folder_path.clear();

    return 0;
}

int files_and_folders::files_list()
{
    DIR *dir = opendir(".");
    struct dirent *d;

    while ((d = readdir(dir)) != NULL)
    {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || 
        strcmp(d->d_name, files_list_name.c_str()) == 0 || strcmp(d->d_name, folders_list_name.c_str()) == 0 || is_bin(d->d_name)) continue;

        if (d->d_type == DT_REG)
        {
            vec_file_path.push_back(d->d_name);
        }            
    }

    closedir(dir);

    return 0;
}

int files_and_folders::write_file_list_in_file()
{
    if(vec_file_path.empty()) return -1;

    if (!bullgnore.empty())
    {
        for (int i = 0; i < bullgnore.size(); i++)
        {
            for (int j = 0; j < vec_file_path.size(); j++)
            {
                if (bullgnore[i] == vec_file_path[j])
                {
                    auto it = vec_file_path.begin() + j;
                    vec_file_path.erase(it);
                }
            }
        }
    }

    if(vec_file_path.empty()) return -1;

    std::ofstream file(files_list_name);

    for (const auto& vec : vec_file_path)
    {
        file << vec << "\n";
    }

    file.close();
    vec_file_path.clear();

    return 0;
}



// INIT
bool _init_::is_BULL()
{
    return (access(config_folder_name, F_OK) == 0);
}

void _init_::fast_collect_data()
{
    files_and_folders ff;
    ff.check_bullgnore();

    ff.dir_list(cur);
    ff.write_dir_list_in_file();

    ff.files_list();
    ff.write_file_list_in_file();
    
    ff.files_list_in_folder();
    ff.write_files_in_folder_list_in_file();
}

int _init_::new_folders_list_name_in_box(int id)
{
    std::string line, path = std::string(config_folder_name) + "/" + std::to_string(id) + "/" + folders_list_name;
    std::ofstream folders_path_for_box; std::ifstream fold_list;

    fold_list.open(folders_list_name);

    if (!fold_list.is_open()) return -1;

    folders_path_for_box.open(path);

    if (!folders_path_for_box.is_open())
    {
        fold_list.close();
        return -1;
    }

    while (std::getline(fold_list, line))
    {
        folders_path_for_box << line << "\n";
    }

    folders_path_for_box.close();
    fold_list.close();

    return 0;
}

int _init_::new_files_box_list_in_box(int id)
{
    std::string line, path = std::string(config_folder_name) + "/" + std::to_string(id) + "/" + files_list_name;
    std::ofstream files_path_for_box; std::ifstream file_list;

    file_list.open(files_list_name);

    if (!file_list.is_open()) return -1;

    files_path_for_box.open(path);

    if (!files_path_for_box.is_open())
    {
        file_list.close();
        return -1;
    }

    while (std::getline(file_list, line))
    {
        files_path_for_box << line << "\n";
    }

    files_path_for_box.close();
    file_list.close();

    return 0;
}

int _init_::create_folders_at_box(int id)
{
    std::ifstream folders(folders_list_name);
    std::string line, fullpath, path = std::string(config_folder_name) + "/" + std::to_string(id) + "/";

    if (!folders.is_open()) return -1;

    while (std::getline(folders, line))
    {
        fullpath =  path + line;
        mkdir(fullpath.c_str(), 0777);
    }

    folders.close();

    if (new_folders_list_name_in_box(id) == 0)
    {
        remove(folders_list_name.c_str());
    }

    return 0;   
}

int _init_::create_files_at_box(int id)
{
    std::string line, text, path = std::string(config_folder_name) + "/" + std::to_string(id) + "/";
    std::ifstream read_file, read_data_file; std::ofstream write_file;
    read_file.open(files_list_name);
    
    if (!read_file.is_open()) return -1;

    while (std::getline(read_file, line))
    {
        read_data_file.open(line);
        if (!read_data_file.is_open()) continue;

        write_file.open(path + line);
        if (!write_file.is_open())
        {
            read_data_file.close();
            continue;
        }

        while (std::getline(read_data_file, text))
        {
            write_file << text << "\n";
        }

        read_data_file.close();
        write_file.close();
        
    }

    read_file.close();

    if (new_files_box_list_in_box(id) == 0)
    {
        remove(files_list_name.c_str());
    }

    return 0;

}

int _init_::generate_id()
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> dist(1000000 , 9000000);

    return dist(gen);
}

int _init_::create_new_box(int id, char *name)
{
    std::ofstream box(std::string(config_folder_name) + "/" + box_list_name, std::ios::app);

    if (!box.is_open()) return -1;

    box << id << " - " << name << "\n";

    box.close();

    std::string fullpath = std::string(config_folder_name) + "/" + std::to_string(id);

    mkdir(fullpath.c_str(), 0777);

    return 0;

}

int _init_::create_folder_for_init()
{
    if (mkdir(config_folder_name, 0777) != 0)
    {
        logger.ERROR("The project was not initiated!");
        return -1;
    }

    logger.INFO("The project has been initiated!");

    std::ofstream box(std::string(config_folder_name) + "/" + box_list_name);

    if (!box.is_open()) return -1;

    box.close();
    return 0;
}

int _init_::add_project(char *folder_name)
{
    if (!is_BULL())
    {
        logger.INFO("Your project has not been initialized!");
        return -1;
    }

    fast_collect_data();

    int id  = generate_id();

    create_new_box(id, folder_name);

    create_folders_at_box(id);
    create_files_at_box(id);

    logger.INFO_NE("The packing '%s' was successful!", folder_name);

    return 0;
}

int _init_::gnore()
{
    std::ofstream gr(bullgnore_name); gr.close();
    logger.INFO("bullgnore has been successfully created!");

    return 0;
}


//ACTION
bool _action_::is_BULL() 
{  
    return (access(config_folder_name, F_OK) == 0); 
}

bool _action_::is_DIR(std::string dir)
{
    return (access(dir.c_str(), F_OK) == 0);
}

void _action_::fast_collect_data()
{
    files_and_folders ff;
    ff.check_bullgnore();

    ff.dir_list(cur);
    ff.write_dir_list_in_file();

    ff.files_list();
    ff.write_file_list_in_file();
    
    ff.files_list_in_folder();
    ff.write_files_in_folder_list_in_file();
}

bool _action_::is_box_list()
{
    std::ifstream bl(std::string(config_folder_name) + "/" + box_list_name);
    if (!bl.is_open()) return false;
    bl.close();
    return true;
}

int _action_::name_to_id(char *name)
{
    struct box_list { std::string id; std::string label; };
    std::string line, word, list[3];
    std::vector<box_list> bl;
    int i = 0;

    std::fstream read_file(std::string(config_folder_name) + "/" + box_list_name);

    if (!read_file.is_open()) return -1;

    while (std::getline(read_file, line))
    {
        std::stringstream ss(line);
        while (ss >> word)
        {
            if (i == 0) list[0] = word;
            if (i == 2) list[2] = word;
            i++;
        }
        i = 0;

        bl.push_back({list[0], list[2]});
    }
    
    read_file.close();

    if (bl.empty()) return -1;

    for (const auto& vec : bl)
    {
        if (vec.label == std::string(name))
        {
            return atoi(vec.id.c_str());
        }
    }

    return 0;
}

std::string _action_::get_last_id()
{
    std::string line, text, format_str;
    std::ifstream r(".bull/" + box_list_name);

    if (!r.is_open()) return "";

    while (std::getline(r, line)) text = line;
    r.close();

    if (text.empty()) return "";

    for (int i = 0; i < text.length(); i++)
    {
        if (text[i] == ' ') break;

        format_str += text[i];
    }

    return format_str;
}

void _action_::delete_data_in_projects()
{
    std::ifstream data;
    std::string line;

    data.open(files_list_name);

    if (data.is_open())
    {
        while (std::getline(data, line))
        {
            remove(line.c_str());
        }
    
        data.close();
    }

    data.open(folders_list_name);

    if (data.is_open())
    {
        while (std::getline(data, line))
        {
            remove(line.c_str());
        }
        
        data.close();
    }

    remove(files_list_name.c_str());
    remove(folders_list_name.c_str());
}

int _action_::delete_box(std::string path)
{
    std::string line, full_path, path_list;
    std::ifstream read_file;

    path_list = path + "/" + files_list_name;
    read_file.open(path_list);

    if (read_file.is_open())
    {
        while (std::getline(read_file, line))
        {
            full_path = path + "/" + line;
            remove(full_path.c_str());
        }

        read_file.close();
        remove(path_list.c_str());
    }

    path_list = path + "/" + folders_list_name;
    read_file.open(path_list);

    if (read_file.is_open())
    {
        while (std::getline(read_file, line))
        {
            full_path = path + "/" + line;
            remove(full_path.c_str());
        }
    
        read_file.close();
        remove(path_list.c_str());
    }

    remove(path.c_str());

    return 0;

}

int _action_::get_line_where_id(std::string id)
{
    std::string line, word;
    std::ifstream r(std::string(config_folder_name) + "/" + box_list_name);
    int count_line = 0;

    if (!r.is_open()) return -1;

    while (std::getline(r, line))
    {
        count_line++;
        std::stringstream ss(line);

        while (ss >> word)
        {
            if (word == id)
            {
                r.close();
                return count_line;
            }
        }
    }

    r.close();

    return 0;
}

void _action_::get_box_list_full()
{
    if (!is_BULL() || !is_box_list())
    {
        logger.INFO("Your project has not been initialized!");
        return;
    }

    std::string line;
    int num = 1;

    std::fstream r(std::string(config_folder_name) + "/" + box_list_name);

    std::cout << "\t\t\t[BOX LIST]\n";

    while (std::getline(r, line))
    {
        std::cout << num++ << ": " << line << "\n";
    }
    
    r.close();
}

void _action_::set(std::string id)
{
    fast_collect_data();
    delete_data_in_projects();
    
    std::string line, line2;
    std::ifstream data, read_file; std::ofstream write_file;

    data.open(std::string(config_folder_name) + "/" + id + "/" + folders_list_name);

    if (data.is_open())
    {
        while (std::getline(data, line))
        {
            mkdir(line.c_str(), 0777);
        }

        data.close();
    }

    data.open(std::string(config_folder_name) + "/" + id + "/" + files_list_name);

    if (data.is_open())
    {
        while (std::getline(data, line))
        {
            read_file.open(std::string(config_folder_name) + "/" + id + "/" + line);
            if (!read_file.is_open()) continue;
            write_file.open(line);
    
            while (std::getline(read_file, line2))
            {
                write_file << line2 << "\n";
            }
    
            read_file.close();
            write_file.close();
        }
        
        data.close();
    }

    logger.INFO_NE("The box '%s' has been successfully unpacked", id.c_str());
}

void _action_::set_box_id(char *str_id)
{
    int id = atoi(str_id);

    if (!is_DIR(std::string(config_folder_name) + "/" + std::to_string(id)))
    {
        logger.ERROR("There is no box with this id!");
        return;
    };

    set(std::to_string(id));
}

void _action_::set_box(char *name)
{
    int id = name_to_id(name);

    if (!is_DIR(std::string(config_folder_name) + "/" + std::to_string(id)))
    {
        logger.ERROR("There is no box with that name!");
        return;
    }

    set(std::to_string(id));
}

void _action_::set_last_box()
{
    std::string id = get_last_id();

    if (id == "")
    {
        logger.ERROR("You don't have any boxes!");
        return;
    }

    set(id);
}

void _action_::update_box_list(std::string id)
{
    std::string line, text = "", path = std::string(config_folder_name) + "/" + box_list_name;
    int pos = get_line_where_id(id), i = 0;
    if (pos == 0 || pos == -1) return;

    std::ifstream data(path);
    std::ofstream new_write;
    if (!data.is_open()) return;

    while (std::getline(data, line))
    {
        i++;
        if (i == pos) continue;
        text += line +  "\n";
    }
    data.close();

    new_write.open(path);

    new_write << text;

    new_write.close();
}

void _action_::delete_last()
{
    std::string id = get_last_id();
    std::string path = std::string(config_folder_name) + "/" + id;

    if (!is_DIR(path))
    {
        logger.ERROR("You don't have a single box!");
        return;
    }

    delete_box(path);
    update_box_list(id);

    logger.INFO("The box was successfully deleted!");
}

void _action_::delete_name(char *name)
{
    int id = name_to_id(name);
    
    std::string path = std::string(config_folder_name) + "/" + std::to_string(id);

    if (!is_DIR(path)) 
    {
        logger.ERROR("There is no box with that name!");
        return;
    }

    delete_box(path);
    update_box_list(std::to_string(id));

    logger.INFO("The box was successfully deleted!");
}

void _action_::delete_id(char *id_)
{
    std::string id = std::string(id_);
    std::string path = std::string(config_folder_name) + "/" + id;

    if (!is_DIR(path))
    {
        logger.ERROR("There is no box with such an ID!");
        return;
    }

    delete_box(path);
    update_box_list(id);

    logger.INFO("The box was successfully deleted!");
}

void _action_::changes(std::string type_file, int mode)
{
    std::string id, path, line;
    id = get_last_id();
    path = std::string(config_folder_name) + "/" + id;
    std::vector<std::string> save_box;
    std::vector<std::string> current_box;
    std::ifstream read_box;
    files_and_folders ff;

    if(!is_DIR(path)) { logger.ERROR("You don't have any boxes!"); return; }

    read_box.open(path + "/" + type_file);

    if (read_box.is_open())
    {
        while (std::getline(read_box, line))
        {
            save_box.push_back(line);
        }

        read_box.close();
    }

    if (mode == 0)
    {
        ff.dir_list(cur);
        ff.write_dir_list_in_file();    
    }
    else if (mode == 1)
    {
        ff.files_list();
        ff.write_file_list_in_file();
        ff.files_list_in_folder();
        ff.write_files_in_folder_list_in_file();
    }
   
    read_box.open(type_file);

    if (read_box.is_open())
    {
        while (std::getline(read_box, line))
        {
            current_box.push_back(line);
        }
        read_box.close();
    }

    if (current_box.empty() && save_box.empty()) return;

    else if (current_box.empty())
    {
        for (const auto& vec : save_box)
        {
            logger.CUSTOM("dark_red", "DEL", vec);
        }
        save_box.clear();
        remove(type_file.c_str());
        return;
    }
    else if (save_box.empty())
    {
        for (const auto& vec : current_box)
        {
            logger.CUSTOM("green", "NEW", vec);
        }
        current_box.clear();
        remove(type_file.c_str());
        return;
    }

    std::unordered_set<std::string> save_set(save_box.begin(), save_box.end());
    std::unordered_set<std::string> current_set(current_box.begin(), current_box.end());

    if (save_set.empty() && current_set.empty()) return;

    for (const auto& item : save_set)
    {
        if (current_set.find(item) == current_set.end())
        {
            logger.CUSTOM("dark_red", "DEL", item);
        }
    }

    for (const auto& item : current_set)
    {
        if (save_set.find(item) == save_set.end())
        {
            logger.CUSTOM("green", "NEW", item);
        }
    }
    remove(type_file.c_str());
}

void _action_::dir_change()
{
    std::cout << "FOLDER:\n";
    changes(folders_list_name, 0);
}

void _action_::file_change()
{
    std::cout << "FILES:\n";
    changes(files_list_name, 1);
}

void _action_::data_change()
{
    std::string id, path, file, old_line, new_line;
    id = get_last_id();
    path = std::string(config_folder_name) + "/" + id;
    std::ifstream new_file, old_file, list;

    if (!is_DIR(path)) { logger.ERROR("You don't have any boxes!"); return; }

    list.open(path + "/" + files_list_name);

    if (!list.is_open()) { logger.WARNING("You don't have any saved files to compare.!"); return; } 

    while (std::getline(list, file))
    {
        old_file.open(path + "/" + file);
        new_file.open(file);

        if (!new_file.is_open() || !old_file.is_open()) continue;

        std::cout << file << ":\n";
        while(std::getline(old_file, old_line))
        {
            while (std::getline(new_file, new_line))
            {
                if (new_line != old_line)
                {
                    if (new_line == "" && old_line != "")
                    {
                        logger.CUSTOM("red", "DEL", old_line);
                    }
                    else if (old_line == "" && new_line != "")
                    {
                        logger.CUSTOM("green", "NEW", new_line);
                    }
                    else if (old_line != "" && new_line != "")
                    {
                        logger.CUSTOM("green", "NEW", new_line);
                    }
                }
            }   
        }
        old_file.close();
        new_file.close();
    }

    list.close();
}

void _action_::all_change()
{
    std::cout << "ALL:\n";
    dir_change();
    file_change();
    data_change();
}