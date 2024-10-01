//
// Created by amir on 02.10.24.
//

#ifndef FIND_MSG_UTILS_H
#define FIND_MSG_UTILS_H


#include <iostream>
#include <vector>
#include <bfd.h>
#include <fstream>
#include <algorithm>

#define STRING_ISGRAPHIC(c) \
      (   (c) >= 0 \
       && (c) <= 255 \
       && ((c) == '\t' || isprint (c) || (c) == '\n')\
	   || ( isspace (c)) \
      )

/* The BFD object file format.  */
static char *target;

/* The BFD section flags that identify an initialized data section.  */
#define DATA_FLAGS (SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS)

/* состояние поиска */
enum States{
    NOT_PRINT,
    ISPRINT
};

using str_buffer = std::vector<std::string>;

/*!
 * Структура для хранения строк,
 * встречающихся в секции ELF
 */
struct section_strings{
    std::string sect_name;
    str_buffer strings;
};

/*!
 * Функция создаст вектор из строк, которые всретятся
 * в буфере *magic - буфере данных конкретной секции
 * @param magiccount - объем буфера magic
 * @param magic - указатель на байты секции
 * @param sec_strings ссылка на вектор строк
 */
void get_string(int magiccount, char *magic, str_buffer& sec_strings){

    std::string str;
    int i = 0;
    int state = STRING_ISGRAPHIC(magic[i]);

    while (i < magiccount){
        switch(state){
            case States::NOT_PRINT:
                while( i < magiccount && ! STRING_ISGRAPHIC(magic[i]))
                    ++i;
                state = States::ISPRINT;
                break;

            case States::ISPRINT:
                while( i < magiccount && STRING_ISGRAPHIC(magic[i]))
                    str.push_back(magic[i++]);
                if(str.size() > 1){
                    sec_strings.emplace_back(str);
                    str.clear();
                }
                state = States::NOT_PRINT;
                break;
        }
    }
    return;
}



/* Заимствовано из исходного кода binutils GNU */
/*!
 * Функция выполнит поиск строк итеративно пройдя по каждой секции
 * @param abfd API библиотеки bfd
 * @param sect указатель на начало списка секций
 * @param str_vector буфер для строк
 */
static void
strings_a_section (bfd *abfd, asection *sect, std::vector<section_strings>& str_vector)
{
    bfd_size_type sectsize;
    bfd_byte *mem;
    section_strings sect_srings;

    sect_srings.sect_name = sect->name;

    if ((sect->flags & DATA_FLAGS) != DATA_FLAGS)
        return;

    sectsize = bfd_section_size (sect);
    if (sectsize == 0)
        return;

    if (!bfd_malloc_and_get_section (abfd, sect, &mem))
    {
        return;
    }

    get_string (sectsize, (char *) mem, sect_srings.strings);

    if( !sect_srings.strings.empty())
        str_vector.emplace_back(sect_srings);

    free (mem);
}



/* Заимствовано из исходного кода binutils GNU */
/*!
 * Функция откроет бинарный файл и вызовет strings_a_section()
 * @param file имя ELF файла
 * @param str_buffer буфер для найденных строк
 * @return успех
 */
static bfd_boolean
strings_object_file (const std::string& file, std::vector<section_strings>& str_buffer)
{
    bfd *abfd;
    asection *s;

    abfd = bfd_openr (file.c_str(), target);

    if (abfd == NULL){
        /* Treat the file as a non-object file.  */
        std:: cout << "Can't open file " << file << std::endl;
        return FALSE;
    }

    /* This call is mainly for its side effect of reading in the sections.
       We follow the traditional behavior of `strings' in that we don't
       complain if we don't recognize a file to be an object file.  */
    if (!bfd_check_format (abfd, bfd_object))
    {
        bfd_close (abfd);
        std:: cout << "This file is not ELF:" << file << std::endl;
        return FALSE;
    }

    for (s = abfd->sections; s != NULL; s = s->next)
        strings_a_section(abfd, s, str_buffer);

    if (!bfd_close (abfd))
    {
        std::cout << "Can't close file correctly"  << std::endl;
        return FALSE;
    }

    return TRUE;
}

/*!
 * Функция наполнит вектор control_list словами, которые необходимо искать в ELF файле,
 * слова берутся из файла с настройками config_file
 * @param control_list ссылка на вектор с контрольными словами
 * @param config_file файл со списком подозрительных слов
 * @return успех
 */
bool get_control_list(std::vector<std::string>& control_list, const std::string config_file){
    std::string line;
    std::ifstream in(config_file); // окрываем файл для чтения
    if (in.is_open())
    {
        while (std::getline(in, line))
        {
            control_list.emplace_back(line);
        }
    }
    else{
        std::cout << "Can't open config file: " << config_file << std::endl;
        return false;
    }
    in.close();
    return true;
}


#endif //FIND_MSG_UTILS_H
