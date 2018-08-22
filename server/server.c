#include "show_bmp.h"
#include "touch.h"
#include "server.h"
#include "show_jpg.h"
#include "video.h"
#include "api_v4l2.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>


//映射内存
unsigned long *p_mem = NULL;
//接收发送方地址
struct sockaddr_in client_addr;
socklen_t addr_len = sizeof(client_addr);

int main()
{
	/* 变量区 */
	int lcd_fd;							//LCD
	int touch_fd;						//触摸屏
	int server_fd;						//服务器
	struct sockaddr_in server_addr;

	/* 初始化区 */
	lcd_fd = lcd_init(&p_mem);			//初始化LCD设备
	touch_fd = touch_init();			//初始化触摸屏
	video_init();						//初始化摄像头

	//创建server的套接字，UDP的数据报
	if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("[FALSE] server socket()");
		return -1;
	}
	else
		printf("[OK] server socket()\n");

	//设置server地址属性
	server_addr.sin_family = AF_INET; 						//地址协议族，ipv4
	server_addr.sin_port = htons(2234);						//设定端口号
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);		//设置IP

	//UDP绑定自己的端口
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		perror("[FALSE] server bind()");
		return -1;
	}
	else
		printf("[OK] server bind()\n");


	//设置线程属性，可分离的
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	//创建线程，指定例程和传入参数
	pthread_t receive_tid;
	pthread_t send_tid;
	pthread_t touch_tid;
	pthread_create(&receive_tid, &attr, receive_routine, (void*)&server_fd);
	pthread_create(&send_tid, &attr, send_routine, (void*)&server_fd);
	pthread_create(&touch_tid, &attr, touch_routine, (void*)&touch_fd);

	//阻塞
	printf("[OK] main server is waitting....\n");
	pause();


	/* 释放区 */
	lcd_close(lcd_fd);					//关闭LCD
	munmap(p_mem, LCD_SIZE);				//解除内存映射
	touch_close(touch_fd);				//关闭触摸屏
	close(server_fd);					//关闭服务器
	video_close();						//关闭摄像头
	printf("[OK] server close()\n");
	return 0;
}

static int BEEP_state = 1;

/**
  * @brief	控制LED1~LED4的开或者关
  * @param	swi：开关标识
  * @retval	None
  */
void led_ctrl(int swi)
{
	int fd;
	fd = open("/dev/Led", O_RDWR);               //打开设备下的LED，成功返回0
	if (fd < 0)
	{
		perror("[FALSE] server open() /dev/LED");
		return;
	}
	else
		printf("[OK] server open() /dev/LED\n");

	ioctl(fd, LED1, swi);
	ioctl(fd, LED2, swi);
	ioctl(fd, LED3, swi);
	ioctl(fd, LED4, swi);
	close(fd);
}

/**
  * @brief	处理手机发送的请求
  * @param	buf：手机请求信息
  * @retval	None
  */
void handle_phone(char *buf)
{
	if (!strcmp(buf, LEN_ON))
	{
		//开LED
		led_ctrl(0);
	}
	else if (!strcmp(buf, LEN_OFF))
	{
		//关LED
		led_ctrl(1);
	}
	else if (!strcmp(buf, BEEP))
	{
		//翻转蜂鸣器
		int fd;
		if ((fd = open("/dev/beep", O_RDWR)) < 0)
		{
			perror("[FALSE] server open() beep");
			return;
		}
		else
			printf("[OK] server open() beep\n");

		ioctl(fd, BEEP_state, 1);
		BEEP_state = !BEEP_state;
		close(fd);
	}
}


/**
  * @brief	线程的执行例程，当线程被创建的时候，可以指定执行例程
  * @param	arg：参数指针
  * @retval	返回void*指针
  */
void *receive_routine(void *arg)
{
	//输出参数
	int *server_fd = (int*)arg;
	printf("[OK] server pthread arg:%d\n", *server_fd);

	//然后接收数据，每次接收128
	char buf[128];
	int size;
	while (1)
	{
		if ((size = recvfrom(*server_fd, 						//socket_fd
		                     buf, 								//数据内容
		                     128, 								//数据长度
		                     0, 									//设为0
		                     (struct sockaddr *)&client_addr,	//发送方的地址
		                     &addr_len)) == -1					//发送方地址结构体大小
		   )
		{
			perror("[FALSE] server recvfrom()");
			break;
		}
		else
		{
			printf("[OK] server recvfrom() size:%d msg:%s\n", size, buf);
			handle_phone(buf);
			memset(buf, 0, sizeof(buf));
		}
	}

	//结束线程
	printf("[OK] pthread exit arg:%d\n", *server_fd);
	pthread_exit(NULL);
}


/**
  * @brief	线程的执行例程，当线程被创建的时候，可以指定执行例程
  * @param	arg：参数指针
  * @retval	返回void*指针
  */
void *send_routine(void *arg)
{
	//输出参数
	int *server_fd = (int*)arg;
	printf("[OK] server pthread arg:%d\n", *server_fd);

	//缓冲区
	FrameBuffer jpg_buf;
	//背景变白
	white(p_mem);
	int send_size;
	while (1)
	{
		//采集的数据放到缓冲区中
		linux_v4l2_get_fream(&jpg_buf);
		show_jpg(NULL, p_mem, 80, 0, jpg_buf.buf, jpg_buf.length);
		show_jpg();
		if ((send_size = sendto(	*server_fd, 						//socket_fd
		                            jpg_buf.buf, 						//数据内容
		                            jpg_buf.length, 					//数据长度
		                            0, 									//设为0
		                            (struct sockaddr *)&client_addr, 	//目标地址
		                            addr_len)) == -1)					//目的地址结构体长度
		{
			perror("[FALSE] client sendto()");
		}
		else
		{
			//printf("[OK] client sendto() size:%d\n",send_size);
		}
	}

	//结束线程
	printf("[OK] pthread exit arg:%d\n", *server_fd);
	pthread_exit(NULL);
}

/**
  * @brief	线程的执行例程，当线程被创建的时候，可以指定执行例程
  * @param	arg：参数指针
  * @retval	返回void*指针
  */
void *touch_routine(void *arg)
{
	//输出参数
	int *touch_fd = (int*)arg;
	printf("[OK] server pthread arg:%d\n", *touch_fd);

	//触摸屏
	int x, y;
	int num = 0;
	while (1)
	{
		get_touch_xy(*touch_fd, &x, &y);
		printf("[OK] touch (%d,%d)\n", x, y);
	}

	//结束线程
	printf("[OK] pthread exit arg:%d\n", *touch_fd);
	pthread_exit(NULL);
}