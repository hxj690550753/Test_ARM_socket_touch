#ifndef SHOW_JPG_H_
#define SHOW_JPG_H_

/**
  * @brief	获取指定路径文件的大小
  * @param	file_path：文件路径
  * @retval	返回文件的大小
  */
unsigned long file_size_get(const char *file_path);

/**
  * @brief	在p_mem映射内存上的坐标(x,y)上画一个color颜色的点
  * @param	p_mem	：映射内存
  * @param	(x,y)	：坐标
  * @param	color	：像素点
  * @retval	None
  */
void lcd_draw_point(unsigned long *p_mem, unsigned int x,unsigned int y, unsigned int color);

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
int show_jpg(const char *jpg_path, unsigned long *p_mem, unsigned int x, unsigned int y, char *jpg_buf,unsigned int jpg_buf_size);



#endif

