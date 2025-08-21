#pragma once
#include <iostream>
#include <list>
#include <string>
#include <time.h>
#include <event.h>
#include <jsoncpp/json/json.h>

class Server; // 前向声明

struct PlayerInfo {
    std::string deviceid;
    std::string appid;
    std::string cur_music;
    int volume;
    int mode;
    time_t d_time;             // 记录音箱上报时间
    time_t a_time;             // 记录APP上报时间

    struct bufferevent *d_bev; // 对应音箱事件
    struct bufferevent *a_bev; // 对应APP事件
};

class Player {
private:
    std::list<PlayerInfo> *info; // 链表对象

public:
    Player();
    ~Player();
    void player_update_list(struct bufferevent *bev, Json::Value &val, Server *server);
    void player_app_update_list(struct bufferevent *bev, Json::Value &val);
    void player_upload_music(struct bufferevent *bev, Json::Value &val, Server *server);
    void player_option(struct bufferevent *bev, Json::Value &val, Server *server);
    void player_reply_option(struct bufferevent *bev, Json::Value &val, Server *server);
    void player_offline(struct bufferevent *bev);
    void player_app_offline(struct bufferevent *bev);
    void player_traverse_list();
    void debug(const char *msg);
};
