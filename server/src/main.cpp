#include <iostream>
#include "server.h"


int main() {

    Server server;    // 创建服务器对象
    server.listen(IP, PORT);  // 监听客户端的链接

    return 0;
}