#include <iostream>
#include "database.h"


Database::Database() {
}

Database::~Database() {
}

bool Database::database_connect() {
    // 初始化数据库句柄
    mysql = mysql_init(NULL);
    if (mysql == NULL) {
        std::cerr << "mysql_init() failed\n";
        return false;
    }
    // 连接数据库
    mysql = mysql_real_connect(mysql, "localhost", "root", "root", "musicplayer", 0, NULL, 0);
    if (mysql == NULL) {
        std::cerr << "mysql_real_connect() failed\n";
        return false;
    }
    // if (mysql_query(mysql, "set name utf8") != 0) {} // 中文设置
    return true;
}

void Database::database_disconnect() {
    if (mysql != NULL) {
        mysql_close(mysql);
        mysql = NULL;
    }
}

bool Database::database_init_table() {
    if (!this->database_connect()) {
        return false;
    }

    const char *sql = "CREATE TABLE IF NOT EXISTS account ("
                      "appid    CHAR(11),"
                      "password VARCHAR(16),"
                      "deviceid VARCHAR(8)"
                      ") CHARSET=utf8;";
    if (mysql_query(mysql, sql) != 0) {
        std::cerr << "mysql_query() failed\n";
        return false;
    }

    this->database_disconnect();

    return true;
}

bool Database::database_user_exists(const std::string &appid) {
    char sql[256] = {0};
    sprintf(sql, "SELECT * FROM account WHERE appid='%s'", appid.c_str());
    if (mysql_query(mysql, sql) != 0) {
        std::cout << "[MYSQL QUERY ERROR]";
        std::cout << mysql_error(mysql) << std::endl;
        return true;
    }
    // 获取查询结果
    MYSQL_RES *res = mysql_store_result(mysql);
    if (res == NULL) {
        std::cout << "[MYSQL STORE RESULT ERROR]";
        std::cout << mysql_error(mysql) << std::endl;
        return true;
    }
    // 查看是否存在
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL) {
        mysql_free_result(res);
        return false;
    }
    else {
        mysql_free_result(res);
        return true;
    }
}

bool Database::database_add_user(const std::string &appid, const std::string &password) {
    char sql[256] = {0};
    sprintf(sql, "INSERT INTO account (appid, password) VALUES ('%s', '%s')", 
        appid.c_str(), password.c_str());
    if (mysql_query(mysql, sql) != 0) {
        std::cout << "[MYSQL QUERY ERROR]";
        std::cout << mysql_error(mysql) << std::endl;
        return false; // 失败
    }
    return true; // 成功
}

bool Database::database_verify_user(const std::string &appid, const std::string &password) {
    char sql[256] = {0};
    // 查询用户对应的密码
    sprintf(sql, "SELECT password FROM account WHERE appid='%s';", appid.c_str());
    if (mysql_query(mysql, sql) != 0) {
        std::cout << "[MYSQL QUERY ERROR]";
        std::cout << mysql_error(mysql) << std::endl;
        return false;
    }
    // 获取查询结果
    MYSQL_RES *res = mysql_store_result(mysql);
    if (res == NULL) {
        std::cout << "[MYSQL STORE RESULT ERROR]";
        std::cout << mysql_error(mysql) << std::endl;
        return false;
    }
    // 查看是否存在，大概率上应该存在
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL) {
        mysql_free_result(res);
        return false;
    }
    // 密码校验
    if (row[0] != NULL && password == row[0]) {
        mysql_free_result(res);
        return true;  // 正确
    }
    mysql_free_result(res);
    return false;  // 不正确
}

bool Database::database_user_bind_check(const std::string &appid, std::string &deviceid) {
    char sql[256] = {0};
    sprintf(sql, "SELECT deviceid FROM account WHERE appid='%s';", appid.c_str());
    if (mysql_query(mysql, sql) != 0) {
        std::cout << "[MYSQL QUERY ERROR]";
        std::cout << mysql_error(mysql) << std::endl;
        return false;
    }
    // 获取查询结果
    MYSQL_RES *res = mysql_store_result(mysql);
    if (res == NULL) {
        std::cout << "[MYSQL STORE RESULT ERROR]";
        std::cout << mysql_error(mysql) << std::endl;
        return false;
    }
    // 查看是否存在，大概率上应该存在
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL) {
        mysql_free_result(res);
        return false;
    }
    // 设备绑定校验
    if (row[0] = NULL) {  // 未绑定
        mysql_free_result(res);
        return false;
    }
    else {                // 绑定
        deviceid = std::string(row[0]);
        mysql_free_result(res);
        return true;
    }
}

bool Database::database_user_bind(const std::string &appid, const std::string &deviceid) {
    char sql[256] = {0};
    sprintf(sql, "UPDATE account SET deviceid='%s' WHERE appid='%s';", 
        deviceid.c_str(), appid.c_str());
    if (mysql_query(mysql, sql) != 0) {
        std::cout << "[MYSQL QUERY ERROR]";
        std::cout << mysql_error(mysql) << std::endl;
        return false;
    }
    return true;
}
