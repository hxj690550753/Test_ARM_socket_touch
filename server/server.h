#ifndef SERVER_H_
#define SERVER_H_

#define MAX_CONNECT_NUM		5
#define PORT 				4321
#define IMG_SIZE			800*480*3
#define SERVER_IP			"192.168.1.138"
#define CLIENT_IP			"192.168.77.140"

#define LEN_ON	"LED_ON"
#define LEN_OFF	"LED_OFF"
#define BEEP	"BEEP"
#define TEST_MAGIC 'x'                           //定义幻数
#define TEST_MAX_NR 2                            //定义命令的最大序数
#define LED1 _IO(TEST_MAGIC, 0)         		//定义LED的魔幻数     
#define LED2 _IO(TEST_MAGIC, 1)
#define LED3 _IO(TEST_MAGIC, 2)
#define LED4 _IO(TEST_MAGIC, 3)

/**
  * @brief	线程的执行例程，当线程被创建的时候，可以指定执行例程
  * @param	arg：参数指针
  * @retval	返回void*指针
  */
extern  void *receive_routine(void *arg);

/**
  * @brief	线程的执行例程，当线程被创建的时候，可以指定执行例程
  * @param	arg：参数指针
  * @retval	返回void*指针
  */
extern  void *send_routine(void *arg);

/**
  * @brief	线程的执行例程，当线程被创建的时候，可以指定执行例程
  * @param	arg：参数指针
  * @retval	返回void*指针
  */
extern  void *touch_routine(void *arg);

/**
  * @brief	创建线程并处理client的请求
  * @param	client_fd：client套接字描述符
  * @retval	返回void*指针
  */
extern void pthread_handle_client(int client_fd);


/**
  * @brief	server初始化，创建socket
  * @param	arg：参数指针
  * @retval	返回void*指针
  */
extern int server_init(void);

#endif
