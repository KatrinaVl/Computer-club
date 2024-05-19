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
    Reception() = default;

    Reception(int64_t n) {
        num_tables = n;
        tables.resize(n + 1);
        free_tables = n;
    }

    std::map<std::string, Client> clients;
    std::queue<Client> waiting;
    std::vector<Table> tables;

    tm_ time_start;
    tm_ time_finish;
    int64_t cost;
    int64_t num_tables;
    int64_t free_tables;

};
