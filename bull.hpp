#ifndef BULL_HPP
#define BULL_HPP

#include <string>
#include <vector>
#include "slog.hpp"

inline const char *config_folder_name = ".bull";
inline const std::string bullgnore_name = ".bullgnore";
inline const std::string folders_list_name = ".___folders_list___";
inline const std::string files_list_name = ".___files_list___";
inline const std::string box_list_name = ".___box_list___";

struct files_and_folders
{
private:
    std::vector<std::string> bullgnore;

public:
    bool is_bin(const std::string& file);

    int check_bullgnore();

    int dir_list(char *path);

    int files_list_in_folder();

    int files_list();
};

class _init_
{
    
private:
    LOG logger;

    bool is_BULL();

    int folder_box_list_create(int id);

    int files_box_list_create(int id);

    int create_folders_at_bull(int id);

    int create_files_at_bull(int id);

    int generate_id();

    int create_box(int id, char *name);

public:
    int create_folder_for_init();

    int add_project(char *folder_name);

    int gnore();
};


class _action_
{
private:
    LOG logger;

    bool is_BULL();

    bool is_box_list();

    int name_to_id(char *name);

    void collect_data_about_files();
    
    void del_current_files_at_dir();

    int comparing_folders_changes(std::string path);

    int comparing_file_changes(std::string path);

    int comparison_of_changes_data_files(std::string id);

    std::string get_last_id();

    int delete_box(std::string path);

    int get_line_where_id(std::string id);

public:  
    void get_box_list_name();

    void get_box_list_id();

    void get_box_list_full();

    void set_box_id(char *str_id);

    void set_box(char *name);

    void pack_last_box();

    void delete_last();

    void delete_name(char *name);

    void delete_id(char *id_);

    void last_change();
};

#endif
