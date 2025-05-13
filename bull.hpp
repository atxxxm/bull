#ifndef BULL_HPP
#define BULL_HPP

#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <dirent.h>
#include <linux/string.h>
#include <vector>
#include <unistd.h>
#include <random>
#include <sstream>
#include <fcntl.h>
#include <cstdint>
#include "slog.hpp"

const char *config_folder_name = ".bull";
const std::string bullgnore_name = ".bullgnore";
const std::string folders_list_name = ".___folders_list___";
const std::string files_list_name = ".___files_list___";
const std::string box_list_name = ".___box_list___";

struct files_and_folders
{
private:
    std::vector<std::string> bullgnore;

public:

    bool is_bin(const std::string& file)
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

    int check_bullgnore()
    {
        bullgnore.clear();
        std::ifstream bg(bullgnore_name);

        if (!bg.is_open()) return -1;

        std::string line;

        while (std::getline(bg, line))
        {
            bullgnore.push_back(line);
        }

        bg.close();

        return 0;
    }

    int dir_list(char *path)
    {
        DIR *dir = opendir(path);

        if (dir == NULL) return -1;

        char fullpath[1024];
        struct dirent *d;
        std::ofstream file(folders_list_name, std::ios::app);

        if (!file.is_open()) return -1;

        while ((d = readdir(dir)) != NULL)
        {
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, config_folder_name) == 0) continue;

            if (!bullgnore.empty())
            {
                for (const auto& vec : bullgnore)
                {
                    if (strcmp(d->d_name, vec.c_str()) != 0)
                    {
                        if (d->d_type == DT_DIR)
                        {
                            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, d->d_name);
                            file << fullpath << std::endl;
                            dir_list(fullpath);
                        }
                    }
                }
            }
            else
            {
                if (d->d_type == DT_DIR)
                {
                    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, d->d_name);
                    file << fullpath << std::endl;
                    dir_list(fullpath);
                }
            }
        }
        
        closedir(dir);
        file.close();

        return 0;
    }

    int files_list_in_folder()
    {
        std::string line, full;
        std::ifstream folder(folders_list_name);
        std::ofstream files(files_list_name, std::ios::app);

        if (!folder.is_open() || !files.is_open())
        {
            folder.close();
            files.close();
            return -1;
        }

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
                    files << full << std::endl;
                }
                
            }

            closedir(dir);
        }

        folder.close();
        files.close();

        return 0;
    }

    int files_list()
    {
        DIR *dir = opendir(".");
        struct dirent *d;
        std::ofstream files(files_list_name);

        if (!files.is_open()) return -1;

        while ((d = readdir(dir)) != NULL)
        {
            if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || 
            strcmp(d->d_name, files_list_name.c_str()) == 0 || strcmp(d->d_name, folders_list_name.c_str()) == 0 || is_bin(d->d_name)) continue;

            if (d->d_type == DT_REG)
            {
                files << d->d_name  << std::endl;
            }            
        }

        closedir(dir);
        files.close();

        return 0;
    }
} ff;

class _init_
{
private:
    LOG logger;

    bool is_BULL() {  return (access(config_folder_name, F_OK) == 0); }

    int folder_box_list_create(int id)
    {
        std::string line;
        std::ofstream folders_path_for_box(std::string(config_folder_name) + "/" + std::to_string(id) + "/" + folders_list_name);
        if (!folders_path_for_box.is_open()) return -1;

        std::ifstream fold_list(folders_list_name);
        if (!fold_list.is_open())
        {
            folders_path_for_box.close();
            return -1;
        }

        while (std::getline(fold_list, line))
        {
            folders_path_for_box << line << std::endl;
        }

        folders_path_for_box.close();
        fold_list.close();

        return 0;
    }

    int files_box_list_create(int id)
    {
        std::string line;
        std::ofstream files_path_for_box(std::string(config_folder_name) + "/" + std::to_string(id) + "/" + files_list_name);
        if (!files_path_for_box.is_open()) return -1;

        std::ifstream file_list(files_list_name);
        if (!file_list.is_open())
        {
            files_path_for_box.close();
            return -1;
        }

        while (std::getline(file_list, line))
        {
            files_path_for_box << line << std::endl;
        }

        files_path_for_box.close();
        file_list.close();

        return 0;
    }

    int create_folders_at_bull(int id)
    {
        std::ifstream folders(folders_list_name);
        std::string line, fullpath;

        if (!folders.is_open()) return -1;

        while (std::getline(folders, line))
        {
            fullpath = std::string(config_folder_name) + "/" + std::to_string(id) + "/" + line;
            mkdir(fullpath.c_str(), 0777);
        }

        folders.close();

        folder_box_list_create(id);

        remove(folders_list_name.c_str());
        return 0;   
    }

    int create_files_at_bull(int id)
    {
        std::string line, text;
        std::ifstream read_file(files_list_name);
        if (!read_file.is_open()) return -1;

        std::ifstream read_data_file;
        std::ofstream write_file;

        while (std::getline(read_file, line))
        {
            read_data_file.open(line);
            if (!read_data_file.is_open()) continue;

            write_file.open(".bull/" + std::to_string(id) + "/" + line);
            if (!write_file.is_open())
            {
                read_data_file.close();
                continue;
            }

            while (std::getline(read_data_file, text))
            {
                write_file << text << std::endl;
            }

            read_data_file.close();
            write_file.close();
            
        }

        read_file.close();

        files_box_list_create(id);

        remove(files_list_name.c_str());

        return 0;

    }

    int generate_id()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
    
        std::uniform_int_distribution<int> dist(1000000 , 9000000);
    
        int num = dist(gen);

        return num;
    }

    int create_box(int id, char *name)
    {
        std::ofstream box(".bull/" + box_list_name, std::ios::app);

        if (!box.is_open()) return -1;

        box << id << " - " << name << std::endl;

        box.close();

        std::string fullpath = config_folder_name + std::string("/") + std::to_string(id);

        mkdir(fullpath.c_str(), 0777);

        return 0;

    }

public:
    int create_folder_for_init()
    {
        if (mkdir(config_folder_name, 0777) != 0)
        {
            logger.ERROR("The project was not initiated!");
            return -1;
        }

        logger.INFO("The project has been initiated!");

        std::ofstream box(config_folder_name + std::string("/") + box_list_name);

        if (!box.is_open()) return -1;

        box.close();
        return 0;
    }

    int add_project(char *folder_name)
    {
        if (is_BULL())
        {
            ff.check_bullgnore();
            char start[1] = {'.'};
            ff.dir_list(start);
    
            ff.files_list();
            ff.files_list_in_folder();

            int id  = generate_id();

            create_box(id, folder_name);
    
            create_folders_at_bull(id);
            create_files_at_bull(id);

            logger.INFO_NE("The packing '%s' was successful!", folder_name);
        }
        else
        {
            logger.INFO("Your project has not been initialized!");
        }
       

        return 0;
    }

    int gnore()
    {
        std::ofstream gr(".bullgnore");
        gr.close();

        logger.INFO("bullgnore has been successfully created!");

        return 0;
    }
    
};


class _action_
{
private:
    LOG logger;

    bool is_BULL() {  return (access(config_folder_name, F_OK) == 0); }

    bool is_box_list()
    {
        std::ifstream bl(std::string(config_folder_name) + "/" + box_list_name);
        if (!bl.is_open()) return false;
        bl.close();
        return true;
    }

    int name_to_id(char *name)
    {
        std::string line, word;
        int i = 0;
        std::string list[3];
        struct brnch_list
        {
            std::string id;
            std::string name;
        };
        
        std::vector<brnch_list> bl;

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
            if (vec.name == std::string(name))
            {
                return atoi(vec.id.c_str());
            }
        }

        return 0;
    }

    void collect_data_about_files()
    {
        char path[1] = {'.'};
        ff.dir_list(path);
        ff.files_list();
        ff.files_list_in_folder();
    }
    
    void del_current_files_at_dir()
    {
        std::ifstream def(files_list_name);
        std::string line;

        while (std::getline(def, line))
        {
            remove(line.c_str());
        }

        def.close();
        def.open(folders_list_name);

        while (std::getline(def, line))
        {
            remove(line.c_str());
        }

        def.close();

        remove(files_list_name.c_str());
        remove(folders_list_name.c_str());
    }

    int comparing_folders_changes(std::string path)
    {
        std::vector<std::string> old_f;
        std::vector<std::string> new_f;
        std::string line;
        std::ifstream read_f(path);

        if (!read_f.is_open()) return -1;

        while (std::getline(read_f, line))
        {
            old_f.push_back(line);
        }

        read_f.close();
        char cur[1] = {'.'};
        ff.dir_list(cur);

        read_f.open(folders_list_name);

        if (!read_f.is_open()) return -1;

        while (std::getline(read_f, line))
        {
            for (int i = 0; i < old_f.size(); i++)
            {
                if (old_f[i] == line)
                {
                    auto it = old_f.begin() + i;
                    old_f.erase(it);
                }
            }

            new_f.push_back(line);
        }

        read_f.close();

        if (!read_f.is_open()) return -1;

        while (std::getline(read_f, line))
        {
            for (int i = 0; i < new_f.size(); i++)
            {
                if (new_f[i] == line)
                {
                    auto it = new_f.begin() + i;
                    new_f.erase(it);
                }
            }
        }

        read_f.close();

        if (old_f.empty() && new_f.empty()) return -1;

        std::cout << "FOLDERS:" << std::endl;
        if (!old_f.empty())
        {
            for (const auto& of : old_f)
            {
                std::cout << "[DEL] "<< of << std::endl;
            }

            std::cout << "\n";
        }   
        
        if (!new_f.empty())
        {

            std::cout << ":" << std::endl;

            for (const auto& nf : new_f)
            {
                std::cout << "[NEW] " << nf << std::endl;
            }
        }

        return 0;
    }

    int comparing_file_changes(std::string path)
    {
        std::vector<std::string> old_f;
        std::vector<std::string> new_f;
        std::string line;
        std::ifstream read_f(path);

        if (!read_f.is_open()) return -1;

        while (std::getline(read_f, line))
        {
            old_f.push_back(line);
        }

        read_f.close();

        ff.files_list();
        ff.files_list_in_folder();

        read_f.open(files_list_name);

        if (!read_f.is_open()) return -1;

        while (std::getline(read_f, line))
        {
            for (int i = 0; i < old_f.size(); i++)
            {
                if (old_f[i] == line)
                {
                    auto it = old_f.begin() + i;
                    old_f.erase(it);
                }
            }

            new_f.push_back(line);
        }

        read_f.close();

        read_f.open(path);

        if (!read_f.is_open()) return -1;

        while (std::getline(read_f, line))
        {
            for (int i = 0; i < new_f.size(); i++)
            {
                if (new_f[i] == line)
                {
                    auto it = new_f.begin() + i;
                    new_f.erase(it);
                }
            }
        }

        read_f.close();

        if (old_f.empty() && new_f.empty()) return -1;

        std::cout << "FILES:" << std::endl;

        if (!old_f.empty())
        {
            for (const auto& of : old_f)
            {
                std::cout << "[DEL] " << of << std::endl;
            }

            std::cout << "\n";
        }

        if (!new_f.empty())
        {
            for (const auto& nf : new_f)
            {
                std::cout << "[NEW] " << nf << std::endl;
            }
        }

        return 0;

    }

    int comparison_of_changes_data_files(std::string id)
    {
        std::vector<std::string> save_vec;
        std::vector<std::string> current_vec;
        std::ifstream file_list;
        std::ifstream r_save;
        std::ifstream r_current;
        std::string current, save, filename;

        file_list.open(files_list_name);

        if (!file_list.is_open()) return -1;

        while (std::getline(file_list, filename))
        {
            r_save.open(".bull/" + id + "/" + filename);
            r_current.open(filename);
            save_vec.clear();
            current_vec.clear();

            if (!r_save.is_open() || !r_current.is_open() || ff.is_bin(filename)) continue;

            while (std::getline(r_save, save))
            {
                if (save == "" || save == "\n") continue;

                save_vec.push_back(save);
            }

            while (std::getline(r_current, current))
            {
                if (current == "" || save == "\n") continue;

                current_vec.push_back(current);
            }

            r_save.close();
            r_current.close();

            r_save.open(".bull/" + id + "/" + filename);
            r_current.open(filename);
            
            while (std::getline(r_current, current))
            {
                if (current == "" || current == "\n") continue;

                for (int i = 0; i < save_vec.size(); i++)
                {
                    if (save_vec[i] == current)
                    {
                        auto it = save_vec.begin() + i;
                        save_vec.erase(it);
                    }
                }
            }
            
            while (std::getline(r_save, save))
            {
                if (save == "" || save == "\n") continue;

                for (int i = 0; i < current_vec.size(); i++)
                {
                    if (current_vec[i] == save)
                    {
                        auto it = current_vec.begin() + i;
                        current_vec.erase(it);
                    }
                }
            }      

            if (save_vec.empty() || current_vec.empty()) continue;

            std::cout << filename << ":" << std::endl;

            for (const auto& sv : save_vec)
            {
                std::cout << "[DEL] " << sv << std::endl;
            }
            std::cout << "\n\n";
            for (const auto& cv : current_vec)
            {
                std::cout << "[ADD] " << cv << std::endl;
            }

            r_save.close();
            r_current.close();
            std::cout << std::endl;
        }

        file_list.close();

        return 0;
    }

    std::string get_last_id()
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

    int delete_box(std::string path)
    {
        std::string line, full_path;
        std::ifstream read_file(path + "/" + files_list_name);

        if (!read_file.is_open()) return -1;

        while (std::getline(read_file, line))
        {
            full_path = path + "/" + line;
            remove(full_path.c_str());
        }

        read_file.close();

        read_file.open(path + "/" + folders_list_name);

        if (!read_file.is_open()) return -1;

        while (std::getline(read_file, line))
        {
            full_path = path + "/" + line;
            remove(full_path.c_str());
        }

        read_file.close();

        full_path = path + "/" + files_list_name;
        remove(full_path.c_str());
        full_path = path + "/" + folders_list_name;
        remove(full_path.c_str());
        remove(path.c_str());

        return 0;

    }

    int get_line_where_id(std::string id)
    {
        std::string line, word;
        std::ifstream r(std::string(config_folder_name) + "/" + box_list_name);
        int count_line = 0;
    
        if (!r.is_open())
        {
            std::cout << "no open!" << std::endl;
            return -1;
        }
    
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

public:  
    void get_box_list_name()
    {
        if (!is_BULL() || !is_box_list()) return;

        std::string line, full;
        int i = 0;
        int num = 1;

        std::fstream r(std::string(config_folder_name) + "/" + box_list_name);
    
        while (std::getline(r, line))
        {
            full += line + "\n";
        }
        
        r.close();
    
        std::stringstream ss(full);
    
        std::cout << "\t\t\t[BOX LIST]" << std::endl;
        while (ss >> line)
        {
            i++;
            if (i == 3)
            {
                std::cout << num++ << ": "<< line << std::endl;
                i = 0;
            }
        }
    }
    
    void get_box_list_id()
    {
        if (!is_BULL() || !is_box_list()) return;

        std::string line, full;
        int i = 0;
        int num = 1;

        std::fstream r(std::string(config_folder_name) + "/" + box_list_name);
    
        while (std::getline(r, line))
        {
            full += line + "\n";
        }
        
        r.close();
    
        std::stringstream ss(full);
    
        std::cout << "\t\t\t[BOX LIST]" << std::endl;

        while (ss >> line)
        {
            if (i == 0)
            {
                std::cout << num++ << ": "<< line << std::endl;
                i = 0;
            }
            i++;
        }
    }

    void get_box_list_full()
    {
        if (!is_BULL() || !is_box_list()) return;

        std::string line;
        int num = 1;

        std::fstream r(std::string(config_folder_name) + "/" + box_list_name);

        std::cout << "\t\t\t[BOX LIST]" << std::endl;
    
        while (std::getline(r, line))
        {
           std::cout << num++ << ": " << line << std::endl;
        }
        
        r.close();
    }

    void set_box_id(char *str_id)
    {
        int id = atoi(str_id);

        std::string line, line2;
        std::ifstream folder_list(std::string(config_folder_name) + "/" + std::to_string(id) + "/" + folders_list_name);

        if (!folder_list.is_open())
        {
            logger.ERROR("There is no box with this id!");
            return;
        }

        collect_data_about_files();
        del_current_files_at_dir();

        while (std::getline(folder_list, line))
        {
            mkdir(line.c_str(), 0777);
        }

        folder_list.close();

        std::ifstream file_list(std::string(config_folder_name) + "/" + std::to_string(id) + "/" + files_list_name);
        std::ifstream read_file;
        std::ofstream write_file;

        while (std::getline(file_list, line))
        {
            read_file.open(std::string(config_folder_name) + "/" + std::to_string(id) + "/" + line);
            write_file.open(line);

            while (std::getline(read_file, line2))
            {
                write_file << line2 << std::endl;
            }

            read_file.close();
            write_file.close();
        }

        file_list.close();

        logger.INFO_NE("The box '%d' has been successfully unpacked", id);
    }

    void set_box(char *name)
    {
        int id = name_to_id(name);

        std::string line, line2;
        std::ifstream folder_list(std::string(config_folder_name) + "/" + std::to_string(id) + "/" + folders_list_name);

        if (!folder_list.is_open())
        {
            logger.ERROR("There is no box with that name!");
            return;
        }

        collect_data_about_files();
        del_current_files_at_dir();

        while (std::getline(folder_list, line))
        {
            mkdir(line.c_str(), 0777);
        }

        folder_list.close();

        std::ifstream file_list(std::string(config_folder_name) + "/" + std::to_string(id) + "/" + files_list_name);
        std::ifstream read_file;
        std::ofstream write_file;

        while (std::getline(file_list, line))
        {
            read_file.open(std::string(config_folder_name) + "/" + std::to_string(id) + "/" + line);
            write_file.open(line);

            while (std::getline(read_file, line2))
            {
                write_file << line2 << std::endl;
            }

            read_file.close();
            write_file.close();
        }

        file_list.close();

        logger.INFO_NE("The box '%s' has been successfully unpacked", name);
    }

    void pack_last_box()
    {
        std::string id = get_last_id();

        std::string line, line2;
        std::ifstream folder_list(std::string(config_folder_name) + "/" + id + "/" + folders_list_name);

        if (!folder_list.is_open()) return;

        collect_data_about_files();
        del_current_files_at_dir();

        while (std::getline(folder_list, line))
        {
            mkdir(line.c_str(), 0777);
        }

        folder_list.close();

        std::ifstream file_list(std::string(config_folder_name) + "/" + id + "/" + files_list_name);
        std::ifstream read_file;
        std::ofstream write_file;

        while (std::getline(file_list, line))
        {
            read_file.open(std::string(config_folder_name) + "/" + id + "/" + line);
            write_file.open(line);

            while (std::getline(read_file, line2))
            {
                write_file << line2 << std::endl;
            }

            read_file.close();
            write_file.close();
        }

        file_list.close();

        logger.INFO("The last box has been successfully unpacked");
    }

    void last_change()
    {
        if (!is_BULL())
        {
            logger.ERROR("The project has not been initialized");
            return;
        }

        if (!is_box_list())
        {
            logger.ERROR("Not box has been added!");
        }

        std::string id = get_last_id();

        if (id == " ") return;

        if (comparing_folders_changes(".bull/" + id + "/.___folders_list___") == 0)
        {
            std::cout << "----------------------------------" << std::endl;
        }
        if (comparing_file_changes(".bull/" + id + "/.___files_list___") == 0)
        {
            std::cout << "----------------------------------" << std::endl;
        }

        comparison_of_changes_data_files(id);

        remove(folders_list_name.c_str());
        remove(files_list_name.c_str());
    }

    void delete_last()
    {
        std::ifstream check(std::string(config_folder_name) + "/" + box_list_name);

        if (!check.is_open())
        {
            logger.ERROR("The project has not been initialized!");
            return;
        }

        check.close();

        std::string id = get_last_id();
        std::string path = std::string(config_folder_name) + "/" + id, str, _str_;
        int i = 0;

        delete_box(path);

        int line = get_line_where_id(id);

        if (line == 0 || line == -1)
        {
            logger.ERROR("Not a single box was found!");
            return;
        }

        std::ifstream read_(std::string(config_folder_name) + "/" + box_list_name);

        if (!read_.is_open()) return;

        while (std::getline(read_, _str_))
        {
            i++;
            if (i == line) continue;
            str += _str_ + "\n";
        }

        read_.close();
        
        std::ofstream write_(std::string(config_folder_name) + "/" + box_list_name);

        if (!write_.is_open()) return;

        write_ << str;

        write_.close();

        logger.INFO("The last box was successfully deleted!");
        
    }

    void delete_name(char *name)
    {
        std::ifstream check(std::string(config_folder_name) + "/" + box_list_name);

        if (!check.is_open())
        {
            logger.ERROR("The project has not been initialized!");
            return;
        }

        check.close();

        int id_ = name_to_id(name);
        std::string id = std::to_string(id_);
        std::string path = std::string(config_folder_name) + "/" + id, str, _str_;
        int i = 0;

        delete_box(path);

        int line = get_line_where_id(id);

        if (line == 0 || line == -1)
        {
            logger.ERROR("Not a single box was found!");
            return;
        }

        std::ifstream read_(std::string(config_folder_name) + "/" + box_list_name);

        if (!read_.is_open()) return;

        while (std::getline(read_, _str_))
        {
            i++;
            if (i == line) continue;
            str += _str_ + "\n";
        }

        read_.close();
        
        std::ofstream write_(std::string(config_folder_name) + "/" + box_list_name);

        if (!write_.is_open()) return;

        write_ << str;

        write_.close();

        logger.INFO("The box by name was successfully deleted!");
        
    }

    void delete_id(char *id_)
    {
        std::ifstream check(std::string(config_folder_name) + "/" + box_list_name);

        if (!check.is_open())
        {
            logger.ERROR("The project has not been initialized!");
            return;
        }

        check.close();

        std::string id = std::string(id_);
        std::string path = std::string(config_folder_name) + "/" + id, str, _str_;
        int i = 0;

        delete_box(path);

        int line = get_line_where_id(id);

        if (line == 0 || line == -1)
        {
            logger.ERROR("Not a single box was found!");
            return;
        }

        std::ifstream read_(std::string(config_folder_name) + "/" + box_list_name);

        if (!read_.is_open()) return;

        while (std::getline(read_, _str_))
        {
            i++;
            if (i == line) continue;
            str += _str_ + "\n";
        }

        read_.close();
        
        std::ofstream write_(std::string(config_folder_name) + "/" + box_list_name);

        if (!write_.is_open()) return;

        write_ << str;

        write_.close();

        logger.INFO("The ID box has been successfully deleted!");
        
    }
};

#endif
