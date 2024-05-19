#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <cctype>

#include "classes.h"

int64_t Check_num(std::string &str_check, std::string &str_out) {
    try {
        if (str_check.find('.') != str_check.npos) {
            std::cout << str_out << std::endl;
            exit(0);
        }
        int64_t f = std::stoll(str_check); // Throws: no conversion
        if (f < 0) {
            std::cout << str_out << std::endl;
            exit(0);
        }

        return f;
    }
    catch (std::invalid_argument const &ex) {
        std::cout << str_out << std::endl;
        exit(0);
    }
}

tm_ Check_time(std::string &str_check, std::string &str_out) {
    if (str_check.size() != 5 || str_check[2] != ':') {
        std::cout << str_out << std::endl;
        exit(0);
    }
    if (!std::isdigit(str_check[0]) || !std::isdigit(str_check[1])
        || !std::isdigit(str_check[3]) || !std::isdigit(str_check[4])) {
        std::cout << str_out << std::endl;
        exit(0);
    }
    if (str_check[0] > '2' || (str_check[0] == '2' && str_check[1] > '3') || str_check[3] > '5') {
        std::cout << str_out << std::endl;
        exit(0);
    }
    int str_h = ((int) str_check[0] - 48) * 10 + ((int) str_check[1] - 48);
    int str_m = ((int) str_check[3] - 48) * 10 + ((int) str_check[4] - 48);
    tm_ t = {str_m, str_h};
    return t;
}

std::string Check_name(std::string &str_check, std::string &str_out) {
    for (auto i: str_check) {
        if (i != '_' && !std::isdigit(i) && !std::isalpha(i)) {
            std::cout << str_out << std::endl;
            exit(0);
        }
    }
    return str_check;
}


Event Check_event(std::string &str_time, std::string &str_ID, std::string &str_name, std::string &str_table,
                  int64_t n_tables, tm_ last_time) {
    std::string except_str = str_time + " " + str_ID + " " + str_name;
    if (str_table != "-1") {
        except_str += " " + str_table;
    }
    tm_ t = Check_time(str_time, except_str);
    int id = Check_num(str_ID, except_str);
    if (id == 0 || id > 4) {
        std::cout << except_str << std::endl;
        exit(0);
    }
    std::string name = Check_name(str_name, except_str);
    int table = -1;
    if (str_table != "-1") {
        table = Check_num(str_table, except_str);
        if (table > n_tables) {
            std::cout << except_str << std::endl;
            exit(0);
        }
    }
    if (t.tm_hour < last_time.tm_hour
        || (t.tm_hour == last_time.tm_hour && t.tm_min < last_time.tm_min)) {
        std::cout << except_str << std::endl;;
        exit(0);
    }
    Event e = {.time = t, .ID_in = id, .num_table = table, .name_of_client = name};
    return e;
}
