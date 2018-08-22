#ifndef TOUCH_H_
#define TOUCH_H_

#define TOUCH_PATH	"/dev/input/event0"


/**
  * @brief  初始化触摸屏，打开触摸屏并配置触摸屏
  * @param  None
  * @retval 返回触摸屏文件描述符
  */
int touch_init(void);

/**
  * @brief  关闭触摸屏
  * @param  ts:文件描述指针
  * @retval None
  */
void touch_close(int fd);

/**
  * @brief  等待触摸屏被触摸
  * @param  touchscreen_fd:触摸屏文件描述符
  * @retval None
  */
void wait_for_touch(int touch_fd);

/**
  * @brief  等待触摸屏被触摸，同时显示触摸坐标值
  * @param  touch_fd:触摸屏文件描述符
  * @retval None
  */
void touch_xy(int touch_fd);

/**
  * @brief  得到触摸屏被触摸的(x,y)坐标
  * @param  touch_fd :触摸屏文件描述符
  * @param  p_x :坐标x的指针
  * @param  p_y :坐标y的指针
  * @retval None
  */
void get_touch_xy(int touch_fd, int *p_x, int *p_y);


#endif
