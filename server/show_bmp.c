#include "show_bmp.h"

#include <stdio.h>			//open、close、perror
#include <unistd.h>			//mmap、lseek、read
#include <sys/mman.h>

#include <sys/types.h>		//opendir、readdir、lseek
#include <dirent.h>

#include <sys/stat.h>		//O_RDONLY、O_RDWR
#include <fcntl.h>

/**
  * @brief  初始化LCD屏幕，并且把LCD设备映射到一块内存上
  * @param  p_mem	:映射内存指针的指针
  * @retval 返回lcd设备文件描述符
  */
int lcd_init(unsigned long **p_mem)
{
	int fd = -1;
	//打开屏幕
	if((fd = open(LCD_PATH,O_RDWR)) == -1)
	{
		perror("open lcd");
		return fd;
	}
	
	//映射内存
	*p_mem =(unsigned long *)mmap(NULL,LCD_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if(*p_mem == MAP_FAILED) 
	{
		perror("mmap lcd");
		close(fd);
	}
	return fd;
}

/**
  * @brief  关闭LCD
  * @param  fd	:lcd设备文件描述符
  * @retval None
  */
void lcd_close(int fd)
{
	close(fd);
}

/**
  * @brief  根据给定的dir读取图片，存放在arr数组内，并统计个数num
  * @param  dir			:图片所在目录
  * @param  picture_arr	:储存图片的数组指针
  * @param  num			:数组长度统计
  * @retval None
  */
void get_picture(const char *dir, char* *picture_arr, int *num)
{
	DIR *dirp = NULL;
	struct dirent *dp = NULL;
	
	if ((dirp = opendir(dir)) == NULL) 		//打开目录指针
	{
		perror("opendir");
		return ;
	}
	readdir(dirp);							//跳过读.
	readdir(dirp);							//跳过读..
	int sum = 0;

	while ((dp = readdir(dirp)) != NULL)   //通过目录指针读目录
	{
		picture_arr[sum++] = dp->d_name;
	}
	*num = sum;
	
	closedir(dirp);							//关闭目录指针
	dirp = NULL;
	dp = NULL;
}

/**
  * @brief  在内存映射指针p_mem基础上，显示以(x,y)为起点
  *			在目标屏幕[width:higth]中显示pathname路径中的图片
  * @param  path	:图片所在目录
  * @param  (x,y)	:起点坐标
  * @param  [w,h]	:屏幕分辨率
  * @param  p_mem	:内存映射指针
  * @retval None
  */
void show_bmp(const char *path,int x,int y,int w,int h,unsigned long *p_mem)
{
	//描述符、遍历变量
	int bmp_fd;
	int i,j;

	//缓存
	char bmp_buffer[w*h*3];
	int lcd_buffer[w*h];


	//打开图片
	if((bmp_fd = open(path,O_RDONLY)) == -1)
	{
		perror("opne bmp");
		return;
	}

	//跳过bmp图片54个字节头
	lseek(bmp_fd,54,SEEK_SET);
	
	//将图片的RGB读到bup_buffer中
	read(bmp_fd,bmp_buffer,sizeof(bmp_buffer));

	//将图片的RGB合成int类型，即一个点
	for(i=0; i<w*h; i++)
		lcd_buffer[i] = bmp_buffer[3*i]|bmp_buffer[3*i+1]<<8|bmp_buffer[3*i+2]<<16;


	//翻转并显示
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
			*(p_mem+(y+i)*800+x+j)=lcd_buffer[(h-1-i)*w+j];


	//关闭BMP
	close(bmp_fd);
}

/**
  * @brief  在内存映射指针p_mem基础上，显示以(x,y)为起点
  *			在目标屏幕[width:higth]中显示pathname路径中的图片
  * @param  bmp_buffer	:图片数组指针
  * @param  (x,y)	:起点坐标
  * @param  [w,h]	:屏幕分辨率
  * @param  p_mem	:内存映射指针
  * @retval None
  */
void display_bmp(char *bmp_buffer,int x,int y,int w,int h,unsigned long *p_mem)
{
	//描述符、遍历变量
	int i,j;

	//缓存
	int lcd_buffer[w*h];

	//将图片的RGB合成int类型，即一个点
	for(i=0; i<w*h; i++)
		lcd_buffer[i] = bmp_buffer[3*i]|bmp_buffer[3*i+1]<<8|bmp_buffer[3*i+2]<<16;


	//翻转并显示
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
			*(p_mem+(y+i)*800+x+j)=lcd_buffer[(h-1-i)*w+j];

}

/**
  * @brief  LCD显示白色
  * @param  None
  * @retval None
  */
void white(unsigned long *p_mem)
{
	show_bmp(BMP_WHITE,0,0,800,480,p_mem);
}
