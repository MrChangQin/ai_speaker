#pragma once
#include <event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <list>
#include <time.h>
#include <jsoncpp/json/json.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include "database.h"
#include "player.h"


#define IP              "172.18.43.68"  // 服务器公网IP地址
#define PORT            8000

#define SEQUENCE        1
#define CIRCLE          2


class Server
{
private:
    struct event_base *m_base; // libevent的事件基础结构（事件集合）
    Database *m_database;      // 数据库对象
    Player *m_player;        // 播放对象

public:
    Server();
    ~Server();

    static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
        struct sockaddr *address, int socklen, void *ctx);
    static void read_cb(struct bufferevent *bev, void *ctx);
    static void event_cb(struct bufferevent *bev, short what, void *ctx);
    static void timeout_cb(evutil_socket_t fd, short events, void *arg);

    void listen(const char *ip, int port);
    struct event_base *server_get_base();
    void server_read_data(struct bufferevent *bev, char *msg);
    void server_send_data(struct bufferevent *bev, Json::Value &val);
    void server_get_music(struct bufferevent *bev, std::string singer);
    void server_app_register(struct bufferevent *bev, Json::Value &val);
    void server_app_login(struct bufferevent *bev, Json::Value &val);
    void server_app_bind(struct bufferevent *bev, Json::Value &val);
    void server_player_handle(struct bufferevent *bev, Json::Value &val);
    void server_client_offline(struct bufferevent *bev);
    void server_app_offline(struct bufferevent *bev);
    void server_start_timer();
};
