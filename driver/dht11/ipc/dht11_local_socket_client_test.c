#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>   // 后续解决Ctrl+C问题需用到的头文件，提前引入
#define SOCKET_PATH "dht11.sock"
 int client_fd = -1;

static void sigint_handler(int sig)
{
    printf("\n Client receive cntr + c: exiting, sig:%d\n", sig);
    if (client_fd > 0) close(client_fd);
    
    exit(-sig);
}

int main(int argc, char **argv) 
{
    
    signal(SIGINT, sigint_handler);
    // 1. 创建UNIX本地域流式套接字，用于与服务端建立连接
    // 函数原型：int socket(int domain, int type, int protocol);
    // AF_UNIX：本地域地址族 | SOCK_STREAM：可靠字节流（TCP特性） | 0：默认协议
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd < 0) {  // 补充原代码缺失的socket创建失败判断
        perror("socket");
        return -1;
    }

    // 2. 初始化服务端地址结构体，指定要连接的服务端套接字文件路径
    struct sockaddr_un addr = {0};          // 结构体初始化，避免脏数据干扰
    addr.sun_family = AF_UNIX;              // 地址族必须与服务端一致，固定为AF_UNIX
    strcpy(addr.sun_path, SOCKET_PATH);     // 填入服务端绑定的套接字文件路径

    // 3. 主动向服务端发起连接请求
    // 函数原型：int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    // 参数：client_fd-客户端套接字 | addr-服务端地址 | sizeof(addr)-地址结构体长度
    // 返回值：成功返回0，失败返回-1并设置errno
    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");  // 打印连接失败原因（如服务端未启动、套接字文件不存在）
        return -1;
    }
  
    printf("local socket connected\n");     // 连接成功提示
    char buf[64];                           // 定义数据接收缓冲区，存放服务端下发的数据
    int n = 0;                              // 保存read函数的实际读取字节数

    // 4. 无限循环，持续接收服务端推送的温度数据
    while (1) {  
        memset(buf, 0, sizeof(buf));        // 每次接收前清空缓冲区，避免残留旧数据
        // 从客户端套接字读取数据，sizeof(buf)-1 预留1字节给'\0'，防止字符串越界
        // read返回值：>0-实际读取字节数 | 0-服务端关闭连接 | <0-读取失败
        if ( (n = read(client_fd, buf, sizeof(buf) - 1)) > 0) {
                            
            printf("receive:%u, %u\n", buf[0], buf[2]);    // 打印接收到的服务端数据
        }
        sleep(2);                           // 每2秒读取一次，降低CPU占用
    }

    return 0;
}