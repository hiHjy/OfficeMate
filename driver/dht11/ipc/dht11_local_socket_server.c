#include <stdio.h>          // 标准输入输出头文件，用于printf、perror等打印函数
#include <sys/socket.h>     // 套接字核心头文件，包含socket、bind、listen、accept、write等接口
#include <sys/un.h>         // UNIX本地域套接字专用头文件，包含sockaddr_un结构体定义
#include <unistd.h>         // 系统调用头文件，包含unlink、close、write等接口
#include <string.h>         // 字符串操作头文件，包含strcpy、snprintf、memset等函数
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEV_NAME   "/dev/dht11"
#define SOCKET_PATH "/tmp/dht11.sock"  // 定义UNIX套接字的文件路径，本地套接字会对应一个磁盘文件
static int client_fd = -1;
static int lsten_fd = -1;

static void sigint_handler(int sig)
{
    printf("\nserver receive Cntr + c: exiting, sig:%d\n", sig);
    if (client_fd > 0) close(client_fd);
    if (lsten_fd > 0) close(lsten_fd);
    exit(-sig);
}
static void sigpipe_handler(int sig)
{
    printf("\n[DHT11 Server] Server process receive :sigpipe (client exitd), sig:%d\n", sig);
    printf("[DHT11 Server]  Waiting for client reconnection\n");
    close(client_fd);
    
}
int main(int argc, char **argv) 
{
    
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, sigpipe_handler);
    
    int fd = open(DEV_NAME, O_RDWR);
    if (fd< 0) {
        printf("can not open file %s, %d\n", DEV_NAME, fd);
        return -1;
    }

    // ===================== 1. 创建UNIX本地域套接字 =====================
    // 函数原型：int socket(int domain, int type, int protocol);
    // 返回值：成功返回 非负的套接字文件描述符；失败返回 -1，errno被设置
    int listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (listen_fd < 0) {          // 套接字创建失败判断
        perror("socket");         // perror：打印最近一次系统调用的错误原因（搭配errno）
        return -1;                // 异常退出程序
    }

    // ===================== 2. 初始化UNIX套接字地址结构体 =====================
    // struct sockaddr_un：UNIX本地域套接字专属地址结构体，替代网络编程的sockaddr_in
    struct sockaddr_un addr = {0};// 初始化结构体，所有成员置0，避免脏数据
    unlink(SOCKET_PATH);          // 关键操作：删除旧的套接字文件
                                  // 原因：UNIX套接字绑定会生成磁盘文件，程序异常退出时文件不会自动删除
                                  // 若文件已存在，后续bind会失败，因此启动时先删除旧文件

    addr.sun_family = AF_UNIX;    // 地址族：必须设置为AF_UNIX（本地域），对应网络编程的AF_INET（IPv4）
    strcpy(addr.sun_path, SOCKET_PATH); // 设置套接字对应的磁盘文件路径

    // ===================== 3. 绑定套接字与本地地址 =====================
    // 函数原型：int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    // 修复原代码BUG：sizeof(addr) < 0 是语法错误，正确写法是 sizeof(addr)
    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");           // 打印绑定失败原因（如文件权限、路径已存在）
        return -1;
    }

    // ===================== 4. 将套接字置为监听状态 =====================
    // 函数原型：int listen(int sockfd, int backlog);
    // 返回值：成功返回0；失败返回-1
    if (listen(listen_fd, 1) < 0) {
        perror("listen");         // 打印监听失败原因
        return -1;                // 原代码缺失该行，监听失败应退出，否则后续accept无意义
    }
    
    printf("dht11 server is listening\n"); // 提示服务器已就绪，等待客户端连接

    // ===================== 5. 阻塞等待客户端连接 =====================
    // 函数原型：int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    // 返回值：成功返回 客户端套接字描述符（用于与客户端通信）；失败返回-1
    // 参数说明：后两个参数传NULL，表示不关心客户端的地址信息（本地套接字客户端无意义）
    unsigned char data[5];
    while (1) {

        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");         // 打印连接失败原因
            return -1;    
        }      
        int n = 0;
        printf("[DHT11 Server]  Client has connected successfully\n") ;
        // ===================== 6. 循环向客户端发送模拟温度数据 =====================
        while (1) {                   // 无限循环，持续推送数据
                        // 定义数据缓冲区，存放要发送的温度字符串
            memset(data, 0, sizeof(data));
            // 格式化写入缓冲区：安全版sprintf，避免缓冲区溢出
            // 函数原型：int snprintf(char *str, size_t size, const char *format, ...);
            if ((n = read(fd, data, sizeof(data))) > 0) {
                //printf("T:%u.%u℃ H:%u.%u%RH\n", data[2], data[3], data[0], data[1]);
                int ret = write(client_fd, data, n);
                if (ret < 0) {
                    perror("write");
                    break;
                }
            }
            //snprintf(buf, sizeof(buf), "TEMP:33\n");
            // 向客户端写入数据
            // 函数原型：ssize_t write(int fd, const void *buf, size_t count);
           
            sleep(2);
        
        }//while



    }//while
   

    return 0;
}