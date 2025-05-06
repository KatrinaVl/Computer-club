#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <cctype>

enum ID_in {
    ID_1 = 1,
    ID_2 = 2,
    ID_3 = 3,
    ID_4 = 4
};

enum ID_out {
    ID_11 = 11,
    ID_12 = 12,
    ID_13 = 13
};

struct tm_ {
    int tm_min;   // minutes of hour from 0 to 59
    int tm_hour;  // hours of day from 0 to 24

    std::string Time_format() {
        std::string res = "";
        if (tm_hour < 10) {
            res += "0" + std::to_string(tm_hour);
        } else {
            res += std::to_string(tm_hour);
        }
        res += ":";
        if (tm_min < 10) {
            res += "0" + std::to_string(tm_min);
        } else {
            res += std::to_string(tm_min);
        }
        return res;
    }
};

struct Client {
    Client() = default;

    Client(std::string name, int64_t n) {
        name_of_client = name;
        num_in_data = n;
    }

    std::string name_of_client;
    int64_t num_table = -1;
    int64_t num_in_data = 0;
};

struct Table {
    tm_ game_time = {0, 0};
    tm_ game_start = {0, 0};
    bool is_occupied = false;
    int64_t table_cost = 0;

};

struct Event {
    tm_ time;
    int ID_in;
    int64_t num_table;
    std::string name_of_client;
    std::string except;
};

class Reception {
public:
    std::map<std::string, Client> clients;
    std::queue<Client> waiting;
    std::vector<Table> tables;

    tm_ time_start;
    tm_ time_finish;
    int64_t cost;
    int64_t num_tables;
    int64_t free_tables;

    Reception() = default;

    Reception(int64_t n) {
        num_tables = n;
        tables.resize(n + 1);
        free_tables = n;
    }

    void PrintOutEvent11(Event& event) {
        std::cout << event.time.Time_format() << " " << ID_out::ID_11 << " ";
        std::cout << event.name_of_client << std::endl;
    }

    void PrintOutEvent12(Event& event, std::string name, int64_t table) {
        std::cout << event.time.Time_format() << " " << ID_out::ID_12 << " ";
        std::cout << name << " " << table << std::endl;
    }

    void PrintOutEvent13(Event& event) {
        std::cout << event.time.Time_format() << " " << ID_out::ID_13 << " ";
        std::cout << event.except << std::endl;
    }

    void DoEvent (Event& event) {
        if (event.ID_in == ID_in::ID_1) {

            if (event.time.tm_hour < time_start.tm_hour
                || (event.time.tm_hour == time_start.tm_hour
                    && event.time.tm_min == time_start.tm_min)) {
                event.except = "NotOpenYet";

                PrintOutEvent13(event);
            } else {
                auto it = clients.find(event.name_of_client);
                if (it != clients.end()) {
                    event.except = "YouShallNotPass";

                    PrintOutEvent13(event);
                } else {
                    int64_t size = clients.size();
                    Client client(event.name_of_client, size);
                    clients[event.name_of_client] = client;
                    waiting.push(client);
                }
            }
        } else if (event.ID_in == ID_in::ID_2) {

            Table &curr_table = tables[event.num_table];
            if (curr_table.is_occupied) {
                event.except = "PlaceIsBusy";

                PrintOutEvent13(event);
            } else {
                auto it = clients.find(event.name_of_client);
                if (it == clients.end()) {
                    event.except = "ClientUnknown";

                    PrintOutEvent13(event);
                } else {
                    Client &curr_client = clients[event.name_of_client];
                    if (curr_client.num_table == -1) {
                        curr_client.num_table = event.num_table;
                        curr_table.is_occupied = true;
                        curr_table.game_start.tm_hour = event.time.tm_hour;
                        curr_table.game_start.tm_min = event.time.tm_min;
                        free_tables -= 1;
                    } else {
                        Table &old_table = tables[curr_client.num_table];
                        Time_of_game(old_table, event.time, cost);
                        old_table.is_occupied = false;
                        curr_client.num_table = event.num_table;
                        curr_table.is_occupied = true;
                        curr_table.game_start = event.time;
                    }
                    if (!waiting.empty()) {
                        waiting.pop();
                    }

                }
            }

        } else if (event.ID_in == ID_in::ID_3) {

            if (free_tables != 0) {
                event.except = "ICanWaitNoLonger!";
                PrintOutEvent13(event);
            } else if (waiting.size() > num_tables) {
                clients.erase(event.name_of_client);
                PrintOutEvent11(event);
            }
        } else if (event.ID_in == ID_in::ID_4) {

            auto it = clients.find(event.name_of_client);
            if (it == clients.end()) {
                event.except = "ClientUnknown";

                PrintOutEvent13(event);
            } else {
                Client &curr_client = clients[event.name_of_client];
                Table &old_table = tables[curr_client.num_table];
                Time_of_game(old_table, event.time, cost);
                old_table.is_occupied = false;
                free_tables += 1;
                if (!waiting.empty()) {
                    auto id = waiting.front().name_of_client;
                    auto it_2 = clients.find(id);
                    waiting.pop();
                    it_2->second.num_table = curr_client.num_table;
                    old_table.is_occupied = true;
                    old_table.game_start.tm_hour = event.time.tm_hour;
                    old_table.game_start.tm_min = event.time.tm_min;
                    free_tables -= 1;

                    PrintOutEvent12(event,it_2->second.name_of_client, it_2->second.num_table);
                }
                curr_client.num_table = -1;
                clients.erase(it);
            }
        }
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


};
