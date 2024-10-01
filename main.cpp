#include "utils.h"


int main(int argc, char *argv[]) {

    std::string binary;                         // имя анализируемого бинарного файла
    std::string config;                         // файл со списком слов для поиска (настройка поиска)
    std::vector<section_strings> buffer_str;    // вектор с найденными строками в бинарном файле, разбивка по секциям
    std::vector<std::string> control_list;      // список слов для поиска

    std::vector<std::string> find_list;         // спиок обнаруженных слов

    /*  Заданы входный бинарный файл и файл со списком слов для поиска */
    if(argc == 3){
        binary = argv[1];
        config = argv[2];
    }
    else{
        std::cout << "Usage: find_msg <path_to_binary> <config_file>";
        return 0;
    }

    if(!get_control_list(control_list, config))
        return 0;

    if( strings_object_file(binary, buffer_str))
        for (auto& [sect_name, strings]: buffer_str)
            for (auto& str: strings)
                for (auto& control_word: control_list) {
                    if(str.find(control_word) != std::string::npos){
                        find_list.emplace_back(str);
                        std::cout << "Find word " << control_word << " in section " << sect_name << std::endl;
                    }
                }

    return 0;

}
