#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "classes.h"
#include "check.h"


int main(int argc, char *argv[]) {
    if (argc <= 1) {
        std::cout << "Give me a file!" << std::endl;
        std::cout << "Format: ./result simple.txt" << std::endl;
        return 0;
    }

    std::ifstream in(argv[1]); // open file for read
    if (in.is_open()) {
        std::string str_check;
        std::string str_check_start, str_check_finish;
        tm_ last_time = {0, 0};
        Event event;

        in >> str_check;
        int64_t n = Check_num(str_check, str_check);

        Reception reception(n);

        in >> str_check_start;
        in >> str_check_finish;
        str_check = str_check_start + " " + str_check_finish;
        reception.time_start = Check_time(str_check_start, str_check);
        reception.time_finish = Check_time(str_check_finish, str_check);

        in >> str_check;
        reception.cost = Check_num(str_check, str_check);
        std::cout << reception.time_start.Time_format() << std::endl;

        while (not in.eof()) {
            std::string str_time, str_ID, str_name;
            std::string str_table = "-1";
            in >> str_time >> str_ID >> str_name;
            if (str_ID == "2") {
                in >> str_table;
                event = Check_event(str_time, str_ID, str_name, str_table, reception.num_tables, last_time);
                std::cout << event.time.Time_format() << " ";
                std::cout << event.ID_in << " " << event.name_of_client;
                std::cout << " " << event.num_table << std::endl;
            } else {
                event = Check_event(str_time, str_ID, str_name, str_table, reception.num_tables, last_time);
                std::cout << event.time.Time_format() << " ";
                std::cout << event.ID_in << " " << event.name_of_client;
                std::cout << std::endl;
            }
            last_time = event.time;
            reception.DoEvent(event);

        }
        for (const auto &cli: reception.clients) {
            Table &old_table = reception.tables[cli.second.num_table];
            reception.Time_of_game(old_table, reception.time_finish, reception.cost);
            std::cout << reception.time_finish.Time_format() << " " << ID_out::ID_11 << " ";
            std::cout << cli.first << std::endl;
        }
        std::cout << reception.time_finish.Time_format() << std::endl;

        for (int64_t i = 1; i < reception.num_tables + 1; ++i) {
            std::cout << i << " " << reception.tables[i].table_cost << " ";
            std::cout << reception.tables[i].game_time.Time_format() << std::endl;
        }
    }
    in.close();

    return 0;
}
