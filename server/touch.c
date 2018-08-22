#include "touch.h"
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>

//ts_read(ts,&sample,1);    //记住这里会产生一个阻塞

/**
  * @brief  初始化触摸屏，打开触摸屏并配置触摸屏
  * @param  None
  * @retval 返回触摸屏文件描述符
  */
int touch_init(void)
{
	int fd = -1;
	if((fd = open(TOUCH_PATH, O_RDONLY)) == -1)
	{
		perror("open touch");
	}
	return fd;
}

/**
  * @brief  关闭触摸屏
  * @param  ts :文件描述指针
  * @retval None
  */
void touch_close(int fd)
{
	close(fd);
}

/**
  * @brief  等待触摸屏被触摸
  * @param  touch_fd :触摸屏文件描述符
  * @retval None
  */
void wait_for_touch(int touch_fd)
{
	struct input_event buf;
	memset(&buf, 0,sizeof (struct input_event));

	while(1)
	{
		read(touch_fd, &buf, sizeof(struct input_event));
		
		//松开
		if(buf.type == EV_ABS  && buf.value == 0)
			return;
	}
}

/**
  * @brief  等待触摸屏被触摸，同时显示触摸坐标值
  * @param  touch_fd :触摸屏文件描述符
  * @retval None
  */
void touch_xy(int touch_fd)
{
	struct input_event buf;
	memset(&buf, 0,sizeof (struct input_event));
	
	unsigned short x = 1000;
	unsigned short y = 1000;

	while(1)
	{
		read(touch_fd, &buf, sizeof(struct input_event));

		if(buf.type == EV_ABS)
		{
			printf("time:%u.%u\t type:EV_ABS\t",buf.time.tv_sec,buf.time.tv_usec);
			switch(buf.code)
			{
				case ABS_X:
					printf("X:%u\n",buf.value);
					x = buf.value;
					break;
				case ABS_Y:
					printf("Y:%u\n",buf.value);
					y = buf.value;
					break;
				case ABS_PRESSURE:
					printf("pressure:%u\n",buf.value);
					break;
				default:
					break;
			}
		}
		else
			if(buf.type == EV_SYN)
			{
					printf("========================SYN====================\n");
			}
		if(x!=1000 && y!=1000)
		{
			printf("(%u,%u)\n",x,y);
			x = 1000;
			y = 1000;
		}
	}	
}

/**
  * @brief  得到触摸屏被触摸的(x,y)坐标
  * @param  touch_fd :触摸屏文件描述符
  * @param  p_x :坐标x的指针
  * @param  p_y :坐标y的指针
  * @retval None
  */
void get_touch_xy(int touch_fd, int *p_x, int *p_y)
{
	struct input_event buf;
	memset(&buf, 0,sizeof (struct input_event));
	
	unsigned short x = 1000;
	unsigned short y = 1000;

	while(1)
	{
		read(touch_fd, &buf, sizeof(struct input_event));

		if(buf.type == EV_ABS)
		{
			switch(buf.code)
			{
				case ABS_X:
					x = buf.value;
					break;
				case ABS_Y:
					y = buf.value;
					break;
				case ABS_PRESSURE:
					break;
				default:
					break;
			}
		}
		if(x!=1000 && y!=1000)
		{
			*p_x = x;
			*p_y = y;
			break;
		}
	}		
}
