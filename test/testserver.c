#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


void *recv_client(void *arg) {
    int sockfd = *(int *)arg;
    char buf[128] = {0};
    while (1) {
        int ret = recv(sockfd, buf, sizeof(buf), 0);
        if (ret <= 0) {
            printf("client closed\n");
        }
        printf("recv data: %s\n", buf);
    }
}

int main(int argc, char **argv) {
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);

    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        return -1;
    }

    struct sockaddr_in server_info;
    memset(&server_info, 0, sizeof(server_info));
    server_info.sin_family = PF_INET;
    server_info.sin_port = htons(8000);
    server_info.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sockfd, (struct sockaddr *)&server_info, sizeof(server_info)) == -1) {
        perror("bind");
        return -1;
    }

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        return -1;
    }

    printf("Listening...\n");
    struct sockaddr_in client_info;
    int client_info_len = sizeof(client_info);
    int clientfd = accept(sockfd, (struct sockaddr *)&client_info, &client_info_len);
    if (clientfd == -1) {
        perror("accept");
        return -1;
    }
    printf("Client connected, fd: %d\n", clientfd);

    pthread_t tid;
    pthread_create(&tid, NULL, recv_client, &clientfd);

    while (1) { 
    }

    return 0;
}