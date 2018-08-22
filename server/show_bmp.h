#ifndef SHOW_BMP_H_
#define SHOW_BMP_H_

#define IMG_NUM_SIZE	64
#define LCD_SIZE		800*480*4
#define LCD_PATH		"/dev/fb0"
#define BMP_DIR			"./bmp_img/"
#define BMP_SIZE		800*480*3
#define BMP_WHITE		"./white.bmp"
 
/**
  * @brief  初始化LCD屏幕，并且把LCD设备映射到一块内存上
  * @param  p_mem	:映射内存指针的指针
  * @retval 返回lcd设备文件描述符
  */
extern int lcd_init(unsigned long **p_mem);

/**
  * @brief  关闭LCD
  * @param  fd	:lcd设备文件描述符
  * @retval None
  */
extern void lcd_close(int fd);

/**
  * @brief  根据给定的dir读取图片，存放在arr数组内，并统计个数num
  * @param  dir			:图片所在目录
  * @param  picture_arr	:储存图片的数组
  * @param  num			:数组长度统计
  * @retval None
  */
extern void get_picture(const char *dir, char* *picture_arr, int *num);

/**
  * @brief  在内存映射指针p_mem基础上，显示以(x,y)为起点
  *			在目标屏幕[width:higth]中显示pathname路径中的图片
  * @param  path	:图片所在目录
  * @param  (x,y)	:起点坐标
  * @param  [w,h]	:屏幕分辨率
  * @param  p_mem	:内存映射指针
  * @retval None
  */
extern void show_bmp(const char *path,int x,int y,int w,int h,unsigned long *p_mem);

/**
  * @brief  服务器接收图片并显示
  * @param  server_fd：服务器fd
  * @retval None
  */
extern void get_BMP(int server_fd, char *img, int size);

/**
  * @brief  LCD显示白色
  * @param  None
  * @retval None
  */
extern void white(unsigned long *p_mem);

#endif

