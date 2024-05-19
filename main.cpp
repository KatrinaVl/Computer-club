#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include "classes.h"
#include "check.h"


std::string Time_format(tm_ time) {
    std::string res = "";
    if (time.tm_hour < 10) {
        res += "0" + std::to_string(time.tm_hour);
    } else {
        res += std::to_string(time.tm_hour);
    }
    res += ":";
    if (time.tm_min < 10) {
        res += "0" + std::to_string(time.tm_min);
    } else {
        res += std::to_string(time.tm_min);
    }
    return res;
}

void Time_of_game(Table &table, tm_ event_time, int64_t real_cost) {
    auto new_time = event_time;
    auto old_time = table.game_start;
    if (new_time.tm_min == old_time.tm_min) {
        table.game_time.tm_hour += new_time.tm_hour - old_time.tm_hour;
        table.table_cost += real_cost * (new_time.tm_hour - old_time.tm_hour);
    } else if (new_time.tm_min < old_time.tm_min) {
        table.game_time.tm_hour += new_time.tm_hour - old_time.tm_hour - 1;
        table.game_time.tm_min += 60 - std::abs(new_time.tm_min - old_time.tm_min);
        if (table.game_time.tm_min >= 60) {
            table.game_time.tm_hour += table.game_time.tm_min / 60;
            table.game_time.tm_min = table.game_time.tm_min % 60;
        }
        table.table_cost += real_cost * (new_time.tm_hour - old_time.tm_hour);
    } else {
        table.game_time.tm_hour += new_time.tm_hour - old_time.tm_hour;
        table.game_time.tm_min += new_time.tm_min - old_time.tm_min;
        table.table_cost += real_cost * (new_time.tm_hour - old_time.tm_hour + 1);
        if (table.game_time.tm_min >= 60) {
            table.game_time.tm_hour += table.game_time.tm_min / 60;
            table.game_time.tm_min = table.game_time.tm_min % 60;
        }
    }
}


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
        std::cout << Time_format(reception.time_start) << std::endl;
        while (not in.eof()) {
            std::string str_time, str_ID, str_name;
            std::string str_table = "-1";
            in >> str_time >> str_ID >> str_name;
            if (str_ID == "2") {
                in >> str_table;
                event = Check_event(str_time, str_ID, str_name, str_table, reception.num_tables, last_time);
                std::cout << Time_format(event.time) << " ";
                std::cout << event.ID_in << " " << event.name_of_client;
                std::cout << " " << event.num_table << std::endl;
            } else {
                event = Check_event(str_time, str_ID, str_name, str_table, reception.num_tables, last_time);
                std::cout << Time_format(event.time) << " ";
                std::cout << event.ID_in << " " << event.name_of_client;
                std::cout << std::endl;
            }
            last_time = event.time;
            if (event.ID_in == ID_in::ID_1) {

                if (event.time.tm_hour < reception.time_start.tm_hour
                    || (event.time.tm_hour == reception.time_start.tm_hour
                        && event.time.tm_min == reception.time_start.tm_min)) {
                    event.except = "NotOpenYet";
                    std::cout << Time_format(event.time) << " " << ID_out::ID_13 << " ";
                    std::cout << event.except << std::endl;
                } else {
                    auto it = reception.clients.find(event.name_of_client);
                    if (it != reception.clients.end()) {
                        event.except = "YouShallNotPass";
                        std::cout << Time_format(event.time) << " " << ID_out::ID_13 << " ";
                        std::cout << event.except << std::endl;
                    } else {
                        int64_t size = reception.clients.size();
                        Client client(event.name_of_client, size);
                        reception.clients[event.name_of_client] = client;
                        reception.waiting.push(client);
                    }
                }
            } else if (event.ID_in == ID_in::ID_2) {

                Table &curr_table = reception.tables[event.num_table];
                if (curr_table.is_occupied) {
                    event.except = "PlaceIsBusy";
                    std::cout << Time_format(event.time) << " " << ID_out::ID_13 << " ";
                    std::cout << event.except << std::endl;
                } else {
                    auto it = reception.clients.find(event.name_of_client);
                    if (it == reception.clients.end()) {
                        event.except = "ClientUnknown";
                        std::cout << Time_format(event.time) << " " << ID_out::ID_13 << " ";
                        std::cout << event.except << std::endl;
                    } else {
                        Client &curr_client = reception.clients[event.name_of_client];
                        if (curr_client.num_table == -1) {
                            curr_client.num_table = event.num_table;
                            curr_table.is_occupied = true;
                            curr_table.game_start.tm_hour = event.time.tm_hour;
                            curr_table.game_start.tm_min = event.time.tm_min;
                            reception.free_tables -= 1;
                        } else {
                            Table &old_table = reception.tables[curr_client.num_table];
                            Time_of_game(old_table, event.time, reception.cost);
                            old_table.is_occupied = false;
                            curr_client.num_table = event.num_table;
                            curr_table.is_occupied = true;
                            curr_table.game_start = event.time;
                        }
                        if (!reception.waiting.empty()) {
                            reception.waiting.pop();
                        }

                    }
                }

            } else if (event.ID_in == ID_in::ID_3) {

                if (reception.free_tables != 0) {
                    event.except = "ICanWaitNoLonger!";
                    std::cout << Time_format(event.time) << " " << ID_out::ID_13 << " ";
                    std::cout << event.except << std::endl;
                } else if (reception.waiting.size() > reception.num_tables) {
                    reception.clients.erase(event.name_of_client);
                    std::cout << Time_format(event.time) << " " << ID_out::ID_11 << " ";
                    std::cout << event.name_of_client << std::endl;
                }
            } else if (event.ID_in == ID_in::ID_4) {

                auto it = reception.clients.find(event.name_of_client);
                if (it == reception.clients.end()) {
                    event.except = "ClientUnknown";
                    std::cout << Time_format(event.time) << " " << ID_out::ID_13 << " ";
                    std::cout << event.except << std::endl;
                } else {
                    Client &curr_client = reception.clients[event.name_of_client];
                    Table &old_table = reception.tables[curr_client.num_table];
                    Time_of_game(old_table, event.time, reception.cost);
                    old_table.is_occupied = false;
                    reception.free_tables += 1;
                    if (!reception.waiting.empty()) {
                        auto id = reception.waiting.front().name_of_client;
                        auto it_2 = reception.clients.find(id);
                        reception.waiting.pop();
                        it_2->second.num_table = curr_client.num_table;
                        old_table.is_occupied = true;
                        old_table.game_start.tm_hour = event.time.tm_hour;
                        old_table.game_start.tm_min = event.time.tm_min;
                        reception.free_tables -= 1;
                        std::cout << Time_format(event.time) << " " << ID_out::ID_12 << " ";
                        std::cout << it_2->second.name_of_client << " " << it_2->second.num_table << std::endl;
                    }
                    curr_client.num_table = -1;
                    reception.clients.erase(it);
                }
            }

        }
        for (const auto &cli: reception.clients) {
            Table &old_table = reception.tables[cli.second.num_table];
            Time_of_game(old_table, reception.time_finish, reception.cost);
            std::cout << Time_format(reception.time_finish) << " " << ID_out::ID_11 << " ";
            std::cout << cli.first << std::endl;
        }
        std::cout << Time_format(reception.time_finish) << std::endl;

        for (int64_t i = 1; i < reception.num_tables + 1; ++i) {
            std::cout << i << " " << reception.tables[i].table_cost << " ";
            std::cout << Time_format(reception.tables[i].game_time) << std::endl;
        }
    }
    in.close();

    return 0;
}
