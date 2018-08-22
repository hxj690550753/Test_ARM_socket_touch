#include "show_jpg.h"


#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "jpeglib.h"

#define LCD_WIDTH  			800
#define LCD_HEIGHT 			480
#define FB_SIZE				(LCD_WIDTH * LCD_HEIGHT * 4)
static char g_color_buf[FB_SIZE] = {0};


/**
  * @brief	获取指定路径文件的大小
  * @param	file_path：文件路径
  * @retval	返回文件的大小
  */
unsigned long file_size_get(const char *file_path)
{
	unsigned long filesize = -1;
	struct stat statbuff;

	if (stat(file_path, &statbuff) >= 0)
	{
		filesize = statbuff.st_size;
	}

	return filesize;
}

/**
  * @brief	在p_mem映射内存上的坐标(x,y)上画一个color颜色的点
  * @param	p_mem	：映射内存
  * @param	(x,y)	：坐标
  * @param	color	：像素点
  * @retval	None
  */
void lcd_draw_point(unsigned long *p_mem, unsigned int x, unsigned int y, unsigned int color)
{
	*(p_mem + y * 800 + x) = color;
}



/**
  * @brief	如果jpg_buf为NULL，则根据jpg_path的路径打开图片，
  *			并在p_mem内存上的(x,y)起点处开始画jpg_buf_size大
  *			的JPG图片。如果jpg_buf不为NULL，就画jpg_buf的内容
  * @param	jpg_path		：JPG图片路径
  * @param	p_mem			：内存映射指针
  * @param	(x,y)			：起始坐标
  * @param	jpg_buf			：摄像头数据
  * @param	jpg_buf_size	：图片大小
  * @retval	成功返回0，失败返回-1
  */
int show_jpg(const char *jpg_path, unsigned long *p_mem, unsigned int x, unsigned int y, char *jpg_buf, unsigned int jpg_buf_size)
{
	/*定义解码对象，错误处理对象*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;

	char 	*pcolor_buf = g_color_buf;
	char 	*p_jpg;

	unsigned int 	i = 0;
	unsigned int	color = 0;
	unsigned int	count = 0;

	unsigned int 	x_s = x;
	unsigned int 	x_e ;
	unsigned int 	y_e ;

	int	jpg_fd;
	unsigned int 	jpg_size;

	unsigned int 	jpg_width;
	unsigned int 	jpg_height;


	//如果不是摄像头数据,指针p_jpg指向jpg_path路径JPG图片
	if (NULL != jpg_path)
	{
		//打开JPG图片
		if ((jpg_fd = open(jpg_path, O_RDWR)) == -1)
		{
			perror("[FALSE] open() jpg");
			return -1;
		}

		//获取jpg文件的大小
		jpg_size = file_size_get(jpg_path);

		//为jpg文件申请内存空间
		p_jpg = malloc(jpg_size);

		//读取jpg文件所有内容到内存
		read(jpg_fd, p_jpg, jpg_size);
	}
	//如果是摄像头数据,指针p_jpg指向摄像头提供的JPG图片
	else
	{
		jpg_size = jpg_buf_size;

		p_jpg = jpg_buf;
	}

	//注册出错处理
	cinfo.err = jpeg_std_error(&jerr);

	//创建解码
	jpeg_create_decompress(&cinfo);

	//直接解码内存数据
	jpeg_mem_src(&cinfo, p_jpg, jpg_size);

	//读文件头
	jpeg_read_header(&cinfo, TRUE);

	//开始解码
	jpeg_start_decompress(&cinfo);

	//打印获取图片的实际高度和宽度
	//printf("cinfo.output_height = %d\n,cinfo.output_width = %d\n",cinfo.output_height,cinfo.output_width);


	x_e	= x_s + cinfo.output_width;
	y_e	= y  + cinfo.output_height;
	//printf("x_e = %d , y_e = %d\n",x_e,y_e);
	//读解码数据
	while (cinfo.output_scanline < cinfo.output_height )
	{
		pcolor_buf = g_color_buf;

		//读取jpg一行的rgb值
		jpeg_read_scanlines(&cinfo, (JSAMPARRAY)&pcolor_buf, 1);

		for (i = 0; i < cinfo.output_width; i++)
		{
			//获取rgb值
			color = 		*(pcolor_buf + 2);
			color = color | *(pcolor_buf + 1) << 8;
			color = color | *(pcolor_buf) << 16;

			//显示像素点
			lcd_draw_point(p_mem, x, y, color);

			pcolor_buf += 3;

			x++;
		}

		//换行
		y++;

		x = x_s;
		//printf("x=%d  y=%d\n",x,y);

	}



	//解码完成
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if (jpg_path != NULL)
	{
		//关闭jpg文件
		close(jpg_fd);

		//释放jpg文件内存空间
		free(p_jpg);
	}
	return 0;
}

