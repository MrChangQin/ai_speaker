#include "player.h"
#include "server.h"


Player::Player() {
    info = new std::list<PlayerInfo>;
}

Player::~Player() {
    if (info) {
        delete info;
        info = nullptr;
    }
}

void Player::player_update_list(bufferevent *bev, Json::Value &val, Server *server) {
    // 遍历链表，如果设备存在，更新链表并转发给服务器
    for (auto it = info->begin(); it != info->end(); ++it) {
        if (it->deviceid == val["deviceid"].asString()) {
            it->cur_music = val["cur_music"].asString();
            it->volume = val["volume"].asInt();
            it->mode = val["mode"].asInt();
            it->d_time = time(NULL);

            if (it->a_bev != NULL) {
                server->server_send_data(it->a_bev, val);
            }
            return;
        }
    }
    // 如果设备不存在，新建节点
    PlayerInfo new_info;
    new_info.deviceid = val["deviceid"].asString();
    new_info.cur_music = val["cur_music"].asString();
    new_info.volume = val["volume"].asInt();
    new_info.mode = val["mode"].asInt();
    new_info.d_time = time(NULL);
    // new_info.a_time = time(NULL);
    new_info.d_bev = bev;
    new_info.a_bev = NULL;

    info->push_back(new_info);
}

void Player::player_app_update_list(bufferevent *bev, Json::Value &val) {
    for (auto it = info->begin(); it != info->end(); ++it) {
        if (it->deviceid == val["deviceid"].asString()) {
            it->appid == val["appid"].asString();
            it->a_time = time(NULL);
            it->a_bev = bev;
        }
    }
}

void Player::player_upload_music(bufferevent *bev, Json::Value &val, Server *server) {
    for (auto it = info->begin(); it != info->end(); ++it) {
        if (it->d_bev == bev) {
            if (it->a_bev != NULL) {   //app在线
                server->server_send_data(it->a_bev, val);
                debug("Music uploaded successfully to app.");
            }
            else {
                debug("Music upload failed: app is offline.");
            }
            break;
        }
    }
}

void Player::player_get_music(bufferevent *bev, Json::Value &val, Server *server) {
    for (auto it = info->begin(); it != info->end(); ++it) {
        if (it->a_bev == bev) {
            server->server_send_data(it->d_bev, val);
            break;
        }
    } 
}

void Player::player_option(bufferevent *bev, Json::Value &val, Server *server) {
    // 判断音箱是否在线
    for (auto it = info->begin(); it != info->end(); ++it) {
        if (it->a_bev == bev) {    // 如果音箱在线，因为有a_bev一定有d_bev
            server->server_send_data(it->d_bev, val);  // 发送指令给音箱
            return;
        }
    }

    // 音箱不在线
    Json::Value reply;
    std::string cmd = val["cmd"].asString();
    cmd += "_reply";
    reply["cmd"] = cmd;
    reply["status"] = "offline";
    server->server_send_data(bev, reply);
    debug("Player option failed: device is offline.");
}

void Player::player_reply_option(bufferevent *bev, Json::Value &val, Server *server) {
    for (auto it = info->begin(); it != info->end(); ++it) {
        if (it->d_bev == bev) {
            if (it->a_bev != NULL) {
                server->server_send_data(it->a_bev, val);
                break;
            }
        }
    }
}

void Player::player_offline(bufferevent *bev) {
    for (auto it = info->begin(); it != info->end(); ++it) {
        if (it->d_bev == bev) {  // 说明是音箱异常下线
            debug("Player offline: device is offline.");
            info->erase(it);  // 删除音箱信息
            break;
        }
        if (it->a_bev == bev) {  // 说明是APP异常下线
            debug("Player offline: app is offline.");
            it->a_bev = NULL;  // 清除APP事件
            break;
        }
    }
}

void Player::player_app_offline(bufferevent *bev) {
    for (auto it = info->begin(); it != info->end(); ++it) {
        if (it->a_bev == bev) {  // 说明是app正常下线
            debug("Player offline: app is offline by user.");
            it->a_bev = NULL;  // 清除APP事件
            break;
        }
    }   
}

void Player::player_traverse_list() {
    debug("Traversing player list...");
    for (auto it = info->begin(); it != info->end(); ++it) {
        // 如果设备超过30秒没有上报，删除该设备
        if (time(NULL) - it->d_time > 6) {  // 超过三次没有上传，判定音箱离线
            std::cout << "[Player Info] Device ID: " << it->deviceid << " has been inactive for too long, removing." << std::endl;
            info->erase(it);
        }
        if (it->a_bev != NULL) {   // APP在线
            if (time(NULL) - it->a_time > 6) {  // APP超过三次没有上报，删除该设备
                std::cout << "[Player Info] Device ID: " << it->deviceid << " has been inactive for too long, removing." << std::endl;
                it->a_bev = NULL;  // 清除APP事件
            }
        }
     }
}

void Player::debug(const char *msg) {
    time_t cur = time(NULL);
    struct tm *tm_info = localtime(&cur);
    std::cout << " [" << tm_info->tm_hour << ":" <<  tm_info->tm_min << ":";
    std::cout << tm_info->tm_sec << "] " << msg << std::endl;
}
