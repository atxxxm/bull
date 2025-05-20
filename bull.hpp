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
inline char cur[1] = {'.'};

struct files_and_folders
{
private:
    std::vector<std::string> bullgnore;
    std::vector<std::string> vec_dir_path;
    std::vector<std::string> vec_file_path;
    std::vector<std::string> vec_file_in_folder_path;

public:
    
    bool is_bin(const std::string& file);

    int check_bullgnore();

    int dir_list(char *path);

    int write_dir_list_in_file();

    int files_list_in_folder();

    int write_files_in_folder_list_in_file();

    int files_list();

    int write_file_list_in_file();
};

class _init_
{
    
private:
    LOG logger;

    bool is_BULL();

    void fast_collect_data();

    int new_folders_list_name_in_box(int id);

    int new_files_box_list_in_box(int id);

    int create_folders_at_box(int id);

    int create_files_at_box(int id);

    int generate_id();

    int create_new_box(int id, char *name);

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

    bool is_DIR(std::string dir);

    void fast_collect_data();

    bool is_box_list();

    int name_to_id(char *name);
    
    std::string get_last_id();

    void delete_data_in_projects();

    int delete_box(std::string path);

    int get_line_where_id(std::string id);

    void set(std::string id);

    void update_box_list(std::string id);

    void changes(std::string type_file, int mode);

public:  
    void get_box_list_full();

    void set_box_id(char *str_id);

    void set_box(char *name);

    void set_last_box();

    void delete_last();

    void delete_name(char *name);

    void delete_id(char *id_);

    void dir_change();

    void file_change();

    void data_change();

    void all_change();
};

#endif
