#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <stdint.h>
#include <poll.h>
static short *screen_base;
static  int lcd_width,lcd_height;
#define RANGE_LIMIT(x) (x > 255 ? 255 : (x < 0 ? 0 : x))
#define FRAME_WIDTH 800
#define FRAME_HEIGHT 480 
#define FRAMEBUFFER_COUNT 2
static int fd,lcd_fd;

static struct buf_info {
	int length;
	short *start;
	
}buf_infos[10];//最大10个buf;

static char v4l2_dev_path[32];
static struct v4l2_dev_info {
	int pixelformat[10];
	char frame_rate;
	
} v4l2_camera;

static void lcd_init()
{
    struct fb_fix_screeninfo fb_fix;
    struct fb_var_screeninfo fb_var;
    unsigned int screen_size;
    /*打开设备文件*/
    lcd_fd = open("/dev/fb0", O_RDWR);
    assert(lcd_fd > 0);
    ioctl(lcd_fd, FBIOGET_VSCREENINFO, &fb_var);
    ioctl(lcd_fd, FBIOGET_FSCREENINFO, &fb_fix);
	fb_var.xres = FRAME_WIDTH;
	fb_var.yres = FRAME_HEIGHT;
	if (ioctl(lcd_fd, FBIOPUT_VSCREENINFO, &fb_var) < 0) {
    	perror("FBIOPUT_VSCREENINFO error");
	}
	ioctl(lcd_fd, FBIOGET_VSCREENINFO, &fb_var);
    printf("--lcd--<%d*%d>\n", fb_var.xres, fb_var.yres);
	
	// printf("lcd:width*height:%d*%d\n",fb_var.xres, fb_var.yres);
    /*映射lCD的缓存区*/
    screen_size = fb_fix.line_length * fb_var.yres;
    screen_base = mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if (screen_base == MAP_FAILED) {
        perror("mmap error");
        close(lcd_fd);
        exit(-1);
    }

   memset(screen_base, 0xFF, screen_size); 

}

void YUV422ToRGB565(const void* inbuf, void* outbuf, int width, int height)
{
	int rows, cols;
	int y, u, v, r, g, b;
	unsigned char *yuv_buf;
	unsigned short *rgb_buf;
	int y_pos,u_pos,v_pos;
 
	yuv_buf = (unsigned char *)inbuf;
	rgb_buf = (unsigned short *)outbuf;
 
	y_pos = 0;
	u_pos = 1;
	v_pos = 3;
 
	for (rows = 0; rows < height; rows++) {
		for (cols = 0; cols < width; cols++) {
			y = yuv_buf[y_pos];
			u = yuv_buf[u_pos] - 128;
			v = yuv_buf[v_pos] - 128;
 
			// R = Y + 1.402*(V-128)
			// G = Y - 0.34414*(U-128)
			// B = Y + 1.772*(U-128)
			r = RANGE_LIMIT(y + v + ((v * 103) >> 8));
			g = RANGE_LIMIT(y - ((u * 88) >> 8) - ((v * 183) >> 8));
			b = RANGE_LIMIT(y + u + ((u * 198) >> 8));
 
			*rgb_buf++ = (((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3));
 
			y_pos += 2;
 
			if (cols & 0x01) {
				u_pos += 4;
				v_pos += 4;
			}
		}
	}
}
static void v4l2_camera_info()
{
	/* 查询支持的像素格式和每种格式下的分辨率 */
	
	char *fmt_info[10]; //存支持的格式
	struct v4l2_fmtdesc fmtdesc;
	fmtdesc.index = 0;	
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	/* 枚举格式 */

	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
		
		printf("index[%d]\t支持格式:0x%x %s\n", fmtdesc.index,fmtdesc.pixelformat,
				 (char *)fmtdesc.description);
		fmt_info[fmtdesc.index] = (char *)fmtdesc.description;
		v4l2_camera.pixelformat[fmtdesc.index] = fmtdesc.pixelformat;
		fmtdesc.index++;
		
		/* 列出当前格式下支持的分辨率 */
		printf("\t\t支持分辨率:");
		struct v4l2_frmsizeenum fsenum = {0}; //存有分辨率信息的结构体
		fsenum.pixel_format = fmtdesc.pixelformat;
		fsenum.index = 0;
		
		while ( ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &fsenum) == 0) {
			fsenum.index++;	
			printf("<%d*%d> ", fsenum.discrete.width, fsenum.discrete.height);
		}
		printf("\n\n");
		
	}



}


static int v4l2_set_format()
{   
    struct v4l2_format fmt = {0};
	int ret = 0;
	printf("开始设置帧格式\n");
    /* 设置帧格式 */
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
    fmt.fmt.pix.width = FRAME_WIDTH;
    fmt.fmt.pix.height = FRAME_HEIGHT ;
    fmt.fmt.pix.pixelformat =  v4l2_camera.pixelformat[1];//yuv

    if (ret = (ioctl(fd, VIDIOC_S_FMT, &fmt)) < 0) {
        perror("ioctl VIDIOC_QUERYCAP error");
   
        return ret;
    }

    // if (fmt.fmt.pix.pixelformat !=  v4l2_camera.pixelformat[1]) {
    //     fprintf(stderr, "像素格是不支持，请选择支持像素格式\n");
    //     return -1;
    // }

    printf("当前视频分辨率为<%d*%d>\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
    

    /* 获取 streamparm 
	struct v4l2_streamparm 是用来设置和获取视频流参数的结构体，主要控制帧率相关的设置。*/

	struct v4l2_streamparm streamparm = {0};
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if ((ret = ioctl(fd, VIDIOC_G_PARM, &streamparm)) < 0) {
        perror("ioctl VIDIOC_G_PARM error"); 
        return ret;
    }
    
    /*检测是否支持帧率设置*/
    if (V4L2_CAP_TIMEPERFRAME & streamparm.parm.capture.capability) {
        //走到这里表示支持帧率设置
        /*设置30fps*/
        printf("该v4l2设备支持帧率设置\n");
        streamparm.parm.capture.timeperframe.denominator = 20;
        streamparm.parm.capture.timeperframe.numerator = 1;
        if ((ret = ioctl(fd, VIDIOC_S_PARM, &streamparm)) < 0) {
            perror("ioctl VIDIOC_S_PARM error");
            return ret;
        }
		v4l2_camera.frame_rate = streamparm.parm.capture.timeperframe.denominator /  
										streamparm.parm.capture.timeperframe.numerator;
		
		printf("当前帧率:[%dFPS]\n", v4l2_camera.frame_rate);

    }

    return 0;
}

static int v4l2_init_buffer()
{
    /*申请缓冲区*/
	int ret;
    struct v4l2_requestbuffers reqbuf = {0};
    struct v4l2_buffer buf = {0};
    reqbuf.count = FRAMEBUFFER_COUNT;
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;

    if ((ret = ioctl(fd, VIDIOC_REQBUFS, &reqbuf)) < 0) {
        perror("ioctl VIDIOC_REQBUFS error");
		return ret;
    }


	 /* 检查实际分配的缓冲区数量 */
    if (reqbuf.count < FRAMEBUFFER_COUNT) {
        printf("警告: 请求%d个缓冲区,实际分配%d个\n", 
               FRAMEBUFFER_COUNT, reqbuf.count);
    }

    if (reqbuf.count < FRAMEBUFFER_COUNT)
    printf("警告: 请求%d个缓冲区,实际分配%d个\n", FRAMEBUFFER_COUNT, reqbuf.count);

    /*建立内存映射*/
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    for (buf.index = 0; buf.index < FRAMEBUFFER_COUNT; ++buf.index) {
        if ((ret = ioctl(fd, VIDIOC_QUERYBUF, &buf)) < 0) {
			perror("ioctl VIDIOC_QUERYBUF error\n");
			return ret;
		}
		
		
		buf_infos[buf.index].length = buf.length;
		buf_infos[buf.index].start = mmap(
										NULL,  
										buf.length,
										PROT_READ |PROT_WRITE, 
										MAP_SHARED, 
										fd,
										buf.m.offset
									);

        if (buf_infos[buf.index].start == MAP_FAILED) {
            fprintf(stderr, "index%d号缓冲区内存映射失败\n", buf.index);
            int i;
            for ( i = 0; i < buf.index; ++i) {
                munmap(buf_infos[i].start, buf_infos[i].length);
                
               
            }
            
            return -1;
        }
        printf("[%d]%p:内存建立映射完成\n",buf.index, buf_infos[buf.index].start);

    }

    
    for(buf.index = 0; buf.index < FRAMEBUFFER_COUNT; ++buf.index) {
        if ((ret = ioctl(fd, VIDIOC_QBUF, &buf)) < 0) {
            perror("ioctl VIDIOC_QBUF error");
    
            return ret;

        }

    }
    
    printf("帧缓存区已准备就绪!\n");

	
    return 0;

}
static void v4l2_print_on_lcd()
{
	struct pollfd fds[1];
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	short out[FRAME_WIDTH * FRAME_HEIGHT ];
	while (1) {

		struct v4l2_buffer buf = {0};
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		int ret = poll(fds,1, 1000);

		if (ret < 0) {
			perror("poll error");
			return;
		} else if (ret == 0) {
			printf("等待帧数据超时\n");
			continue;
		} else {
			if (fds->revents & POLLIN) {
				int ret = ioctl(fd, VIDIOC_DQBUF, &buf);
				if (ret < 0) {
					perror("ioctl VIDIOC_DQBUF");
				}
				
				YUV422ToRGB565(buf_infos[buf.index].start, out, FRAME_WIDTH, FRAME_HEIGHT );
				memcpy(screen_base, out, buf_infos[buf.index].length);
				if ((ret = ioctl(fd, VIDIOC_QBUF, &buf)) < 0) {
					perror("ioctl VIDIOC_QBUF error");

				}

			
			}
		}

		usleep(20000);
		

   }
   
	
}
static int v4l2_stream_on(void) 
{
    /* 打开摄像头、摄像头开始采集数据 */
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(fd, VIDIOC_STREAMON, &type) < 0) {
        perror("ioctl VIDIOC_STREAMON error");
        close(fd);
        return -1;
    }
    printf("开始视频采集\n");
    return 0;
}
static int v4l2_camera_init()
{
		
	int ret;
		
	/* 查询设备是否为捕获设备 */
	
	struct v4l2_capability capability = {0};
	if ((ret = ioctl(fd, VIDIOC_QUERYCAP, &capability)) < 0) {
			printf("ioctl VIDIOC_QUERYCAP error:%s\n",strerror(errno));	
	}

	if (!(capability.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		printf("非视频采集设备\n");
		goto err;
	
	} else {
		printf("%s 确认为视频采集设备\n",v4l2_dev_path);
	}
	
	printf("--开始初始化设备--\n");

	v4l2_camera_info(); //查询摄像头支持的像素格式和分辨率
	if ((ret = v4l2_set_format()) < 0) {
		goto err;
	}
	if (ret = v4l2_init_buffer() < 0) {
		goto err;
	}
	return 0;

err:
	close(fd);
	return ret;

}




int main (int argc, char **argv)
{
	int ret;
	if (argc != 2) {
		printf("Usage:%s </dev/videoX>\n");
		return -1;
	}
	fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		printf("can not open device %s\n", argv[1]);
		return -1;
	}

	strcpy(v4l2_dev_path,argv[1]);

	if ( (ret = v4l2_camera_init()) < 0) {
		printf("v4l2_camera_init error:ret = %d\n");
		return -1;
	}
	
	lcd_init();
	v4l2_stream_on();
	v4l2_print_on_lcd();
	return 0;
}








