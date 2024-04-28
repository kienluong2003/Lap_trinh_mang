#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


void handle_client(int client) {
    // Nhận dữ liệu từ client và in ra màn hình
    char buf[256];
    int ret = recv(client, buf, sizeof(buf), 0);
    buf[ret] = '\0';
    printf("%s\n", buf);

    // Trả lại kết quả cho client
    char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Xin chao cac idol</h1></body></html>";
    send(client, msg, strlen(msg), 0);

    // Đóng kết nối
    close(client);
}

int main() {
    // Khởi tạo server
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(9000);

    bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(listener, 10);

    // Fork các tiến trình con
    for (int i = 0; i < 8; i++) {
        if (fork() == 0) {
            // Tiến trình con
            while (1) {
                // Chờ kết nối mới
                int client = accept(listener, NULL, NULL);
                printf("New client connected: %d\n", client);

                // Xử lý client
                handle_client(client);
            }
            break;
        }
    }

    // Tiến trình cha
    for (int i = 0; i < 8; i++) {
        wait(NULL);
    }

    return 0;
}