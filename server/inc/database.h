#pragma once

#include <iostream>
#include <mysql/mysql.h>


class Database
{
private:
    MYSQL *mysql;   // 数据库句柄

public:
    Database();
    ~Database();
    bool database_connect();
    void database_disconnect();
    bool database_init_table();
    bool database_user_exists(const std::string &appid);
    bool database_add_user(const std::string &appid, const std::string &password);
    bool database_verify_user(const std::string &appid, const std::string &password);
    bool database_user_bind_check(const std::string &appid, std::string &deviceid);
    bool database_user_bind(const std::string &appid, const std::string &deviceid);
};

