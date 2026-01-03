#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEV_NAME   "/dev/dht11"



int main(int argc, char **argv)
{
 
    int ret;

    
    /* 2. 打开文件 */
    int fd = open(DEV_NAME, O_RDWR);   // | O_NONBLOCK
    
    if (fd< 0)
    {
        printf("can not open file %s, %d\n", DEV_NAME, fd);
        return -1;
    }
    unsigned char buf[5];
   
    while (1)
    {   
       
        if (read(fd, &buf, sizeof(buf)) > 0) {
        //    printf("温度:%d \n 湿度:%d\n", buf[1], buf[2]);
        // }  else {
        //     printf("raed error\n");
            printf("湿度:%u.%u 温度:%u.%u \n", buf[0], buf[1], buf[2], buf[3]);
        }
        usleep(1500000);
    }
}