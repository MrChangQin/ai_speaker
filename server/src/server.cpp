#include <iostream>
#include "server.h"


Server::Server() {
    // 初始化事件集合
    m_base = event_base_new();

    // 初始化数据库
    m_database = new Database();
    if (!m_database->database_init_table()) {
        std::cerr << "Failed to initialize database table" << std::endl;
        delete m_database;
        m_database = nullptr;
        exit(1);
    }

    // 创建player对象
    m_player = new Player();
}

Server::~Server() {
    if (m_database) {
        delete m_database;
        m_database = nullptr;
    }
    if (m_player) {
        delete m_player;
        m_player = nullptr;
    }
}

void Server::listen(const char *ip, int port) {
    struct sockaddr_in server_info;
    int len = sizeof(server_info);
    memset(&server_info, 0, len);
    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(port);
    server_info.sin_addr.s_addr = inet_addr(ip);

    // 创建监听对象
    struct evconnlistener *listener = evconnlistener_new_bind(m_base, listener_cb, this,
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, 5,
        (struct sockaddr *) &server_info, len);
    if (!listener) {
        std::cerr << "Failed to create listener" << std::endl;
        return;
    }

    // 启动定时器，并开始监听
    server_start_timer();

    // 释放对象
    evconnlistener_free(listener);
    event_base_free(m_base);
}

// 一旦有客户端发起连接请求，就会触发该函数
void Server::listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
    struct sockaddr *address, int socklen, void *ctx) {
    // 处理新连接
    struct sockaddr_in *client_info = (struct sockaddr_in *)address;
    // struct event_base *base = (struct event_base *)ctx;
    Server *server = (Server *)ctx;

    std::cout << "[New client connection] ";
    std::cout << inet_ntoa(client_info->sin_addr) << ":";
    std::cout << client_info->sin_port << std::endl;

    struct bufferevent *bev = bufferevent_socket_new(server->server_get_base(), fd, BEV_OPT_CLOSE_ON_FREE);
    if (bev == NULL) {
        std::cerr << "Failed to create bufferevent" << std::endl;
        return;
    }

    bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);
    bufferevent_enable(bev, EV_READ);
}

// 有客户端发数据过来，触发
void Server::read_cb(struct bufferevent *bev, void *ctx) {
    Server *server = (Server *)ctx;
    char buf[1024] = {0};
    server->server_read_data(bev, buf);

    // 解析json
    Json::Reader reader;   // 用于解析的对象
    Json::Value value;     // 存放解析的结果

    if (reader.parse(buf, value)) {
        // 解析成功
        std::cout << "[Parsed JSON] " << value << std::endl;
    } else {
        // 解析失败
        std::cerr << "[Failed to parse JSON] " << reader.getFormattedErrorMessages() << std::endl;
        return;
    }

    if (value["cmd"] == "get_music_list") {
        // 处理获取音乐列表的命令
        server->server_get_music(bev, value["singer"].asCString());
    }
    else if (value["cmd"] == "app_register") {
        // 处理注册命令
        server->server_app_register(bev, value);
    }
    else if (value["cmd"] == "app_login") {
        // 处理登录命令
        server->server_app_login(bev, value);
    }
    else if (value["cmd"] == "app_bind") {
        // 处理设备绑定命令
        server->server_app_bind(bev, value);
    }
    else if (value["cmd"] == "app_offline") {
        // 处理设备下线命令
        server->server_app_offline(bev);
    }
    else {
        server->server_player_handle(bev, value);
    }
}

// 异常下线判断
void Server::server_client_offline(struct bufferevent *bev) {
    m_player->player_offline(bev);
}

// APP正常下线
void Server::server_app_offline(struct bufferevent *bev) {
    m_player->player_app_offline(bev);
}

// 发生异常时，触发
void Server::event_cb(struct bufferevent *bev, short what, void *ctx) {
    Server *server = (Server *)ctx;
    if (what & BEV_EVENT_EOF) {   // BEV_EVENT_EOF异常
        server->server_client_offline(bev);  // 判断是音箱还是app下线，执行对应操作
    }
}

struct event_base *Server::server_get_base() {
    return m_base;
}

void Server::server_read_data(struct bufferevent *bev, char *msg) {
    // [消息长度(4字节)][实际消息内容(长度字节)]
    char buf[8] = {0};
    size_t size = 0;
    while (true) {
        size += bufferevent_read(bev, buf + size, 4 - size);
        if (size >= 4)
            break;
    }

    int len = *(int *)buf;
    size = 0;
    while (true) {
        size += bufferevent_read(bev, msg + size, len - size);
        if (size >= len)
            break;
    }
    std::cout << "[Read data]: " << msg << "LEN: " << len << std::endl;
}

void Server::server_send_data(struct bufferevent *bev, Json::Value &val) {
    char msg[1024] = {0};
    std::string SendStr = Json::FastWriter().write(val);
    int len = SendStr.size();
    memcpy(msg, &len, sizeof(int));
    memcpy(msg + sizeof(int), SendStr.c_str(), len);

    if (bufferevent_write(bev, msg, len + sizeof(int)) == -1) {
        std::cerr << "[Failed to send data]" << std::endl;
    }
}

void Server::server_get_music(struct bufferevent *bev, std::string singer) {
    Json::Value val;
    Json::Value arr;
    std::list<std::string> music_list;
    std::string path = "/var/www/html/music/" + singer;

    DIR *dir = opendir(path.c_str());
    if (dir == NULL) {
        perror("opendir");
        return;
    }
    struct dirent *d;
    while ((d = readdir(dir)) != NULL) {
        if (d->d_type != DT_REG) {  // 如果不是普通文件
            continue;
        }
        if (!strstr(d->d_name, ".mp3")) { // 如果不是mp3文件
            continue;
        }
        std::string name = singer + "/" + d->d_name;
        music_list.push_back(name);  // 添加到音乐列表
    }

    // 随机选取五首歌
    auto it = music_list.begin();
    srand((unsigned int)time(NULL));
    int count = rand() % (music_list.size() - 4);
    for (int i = 0; i < count; i++) {
        it++;
    }
    for (int i = 0; i < 5 && it != music_list.end(); i++, it++) {
        arr.append(*it);
    }

    val["cmd"] = "reply_music";
    val["music"] = arr;
    server_send_data(bev, val);
}

void Server::server_app_register(struct bufferevent *bev, Json::Value &val) {
    // 连接数据库
    m_database->database_connect();
    // 判断用户是否存在
    std::string appid = val["appid"].asCString();
    std::string password = val["password"].asCString();
    if (m_database->database_user_exists(appid)) {
        // 如果存在，返回错误信息
        val["result"] = "failure";
        std::cout << "[App Register] User already exists: " << appid << std::endl;
    } else {
        // 如果不存在，修改数据库
        m_database->database_add_user(appid, password);
        val["result"] = "success";
        std::cout << "[App Register] New user registered: " << appid << std::endl;
    }
    // 断开数据库
    m_database->database_disconnect();

    // 回复
    val["cmd"] = "app_register_reply";
    server_send_data(bev, val);
}

void Server::server_app_login(struct bufferevent *bev, Json::Value &val) {
    // 连接数据库
    m_database->database_connect();

    Json::Value new_val;
    std::string appid = val["appid"].asCString();
    std::string password = val["password"].asCString();
    std::string deviceid;
    // 校验
    do {
        // 判断用户是否存在
        if (!m_database->database_user_exists(appid)) { // 如果不存在，返回错误信息
            new_val["result"] = "not_exist";
            std::cout << "[App Login] User not found: " << appid << std::endl;
            break;
        } 
        // 如果存在，判断密码是否正确
        if (!m_database->database_verify_user(appid, password)) {  // 如果密码不正确
            new_val["result"] = "password_error";
            std::cout << "[App Login] Password incorrect: " << appid << std::endl;
            break;
        }
        // 是否绑定
        if (m_database->database_user_bind_check(appid, deviceid)) {  // 如果已经绑定
            new_val["result"] = "bind";
            new_val["deviceid"] = deviceid;
            std::cout << "[App Login] User logged in: " << appid << std::endl;
        } else {                                                // 没绑定
            new_val["result"] = "not_bind";
            std::cout << "[App Login] Device not bind: " << appid << std::endl;
        }
    } while(0);

    // 断开数据库
    m_database->database_disconnect();

    // 回复
    new_val["cmd"] = "app_login_reply";
    server_send_data(bev, new_val);
}

void Server::server_app_bind(struct bufferevent *bev, Json::Value &val) {
    std::string appid = val["appid"].asCString();
    std::string deviceid = val["deviceid"].asCString();
    // 连接数据库
    m_database->database_connect();
    // 修改数据库
    m_database->database_user_bind(appid, deviceid);
    // 关闭数据库
    m_database->database_disconnect();
    // 回复
    Json::Value new_val;
    val["cmd"] = "app_bind_reply";
    val["result"] = "success";
    server_send_data(bev, val);
}

void Server::server_player_handle(struct bufferevent *bev, Json::Value &val) {
    if (val["cmd"] == "info") {
        m_player->player_update_list(bev, val, this);
    }
    else if (val["cmd"] == "app_info") {
        m_player->player_app_update_list(bev, val);
    }
    else if (val["cmd"] == "upload_music") {
        m_player->player_upload_music(bev, val, this);
    }
    else if (val["cmd"] == "app_get_music") {
        m_player->player_get_music(bev, val, this);
    }
    else if (val["cmd"] == "app_start"    || val["cmd"] == "app_stop"       || 
             val["cmd"] == "app_suspend"  || val["cmd"] == "app_continue"   || 
             val["cmd"] == "app_prior"    || val["cmd"] == "app_next"       ||
             val["cmd"] == "app_voice_up" || val["cmd"] == "app_voice_down" ||
             val["cmd"] == "app_circle"   || val["cmd"] == "app_sequence") {
        m_player->player_option(bev, val, this);  // bev代表app
    }
    else if (val["cmd"] == "app_start_reply"    || val["cmd"] == "app_stop_reply"       || 
             val["cmd"] == "app_suspend_reply"  || val["cmd"] == "app_continue_reply"   || 
             val["cmd"] == "app_prior_reply"    || val["cmd"] == "app_next_reply"       ||
             val["cmd"] == "app_voice_up_reply" || val["cmd"] == "app_voice_down_reply" ||
             val["cmd"] == "app_circle_reply"   || val["cmd"] == "app_sequence_reply") {
        m_player->player_reply_option(bev, val, this);  // bev代表音箱
    }
}

void Server::server_start_timer() {
    struct event *timer_event = event_new(m_base, -1, EV_PERSIST, timeout_cb, m_player);
    if (!timer_event) {
        std::cerr << "Failed to create timer event" << std::endl;
        return;
    }

    struct timeval tv;
    evutil_timerclear(&tv);
    tv.tv_sec = 2;    // 两秒
    tv.tv_usec = 0;   // 微秒
    
    if (event_add(timer_event, &tv) < 0) {
        std::cerr << "Failed to add timer event" << std::endl;
        event_free(timer_event);
        return;
    }

    // 开始监听
    event_base_dispatch(m_base);   // 死循环
}

void Server::timeout_cb(evutil_socket_t fd, short events, void *arg) {
    Player *player = (Player *)arg;
    player->player_traverse_list();
}
