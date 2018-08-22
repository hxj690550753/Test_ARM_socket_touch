#ifndef CLIENT_H_
#define CLIENT_H_

#define PORT 			4321
#define SERVER_IP		"192.168.77.138"
#define CLIENT_IP		"192.168.77.140"
#define IMG_SIZE		800*480*3
#define BMP_PATH		"./bmp_img/a.bmp"
#define JPG_PATH		"./jpg_img/c.jpg"

/**
  * @brief	线程的执行例程，当线程被创建的时候，可以指定执行例程
  * @param	arg：参数指针
  * @retval	返回void*指针
  */
extern void *routine(void *arg);

#endif
