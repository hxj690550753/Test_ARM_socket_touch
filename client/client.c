#include "client.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
	//client套接字
	int client_fd;
	socklen_t addr_len;
	struct sockaddr_in client_addr;

	//创建UDP套接字
	if((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("[FALSE] client socket()");
		return -1;
	}
	else
		printf("[OK] client socket()\n");

	//设置client的socket属性
	client_addr.sin_family = AF_INET; 						//地址协议族，ipv4
	client_addr.sin_port = htons(PORT);						//设定端口号
	client_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);		//设置IP

	//UDP绑定自己的端口
	if(bind(client_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) == -1)
	{
		perror("[FALSE] client bind()");
		return -1;
	}
	else
		printf("[OK] client bind()\n");


	//创建线程，指定例程和传入参数
	pthread_t tid;
	pthread_create(&tid,NULL,routine,(void*)&client_fd);

	//阻塞等待指定的线程退出，并获取退出值
	void *p = NULL;
	pthread_join(tid,&p);
	printf("[OK] main join pthread msg:%s",(char*)p);


	//关闭套接字
	close(client_fd);
	printf("[OK] client close()\n");
	return 0;
}

/**
  * @brief	线程的执行例程，当线程被创建的时候，可以指定执行例程
  * @param	arg：参数指针
  * @retval	返回void*指针
  */
/*
int sendto(	int s, 							//socket_fd
			const void * msg, 				//数据内容
			int len, 						//数据长度
			unsigned int flags, 			//设为0
			const struct sockaddr * to, 	//目标地址
			int tolen);						//目的地址结构体长度
*/
void *routine(void *arg)
{
	struct sockaddr_in server_addr;

	//设置server地址属性
	server_addr.sin_family = AF_INET; 							//地址协议族，ipv4
	server_addr.sin_port = htons(PORT);							//设定端口号
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);			//设置IP
	socklen_t addr_len = sizeof(server_addr);

	//输出参数
	int *client_fd = (int*)arg;
	printf("[OK] client pthread arg:%d\n",*client_fd);

	//描述符、遍历变量
	int jpg_fd;
	int i,j;

	//打开图片
	if((jpg_fd = open(JPG_PATH,O_RDONLY)) == -1)
	{
		perror("[FALSE] client open jpg");
		return;
	}
	else
		printf("[OK] client open jpg\n");


	//将图片的信息读到jpg_buffer中
	char jpg_buffer[IMG_SIZE];					//图片数据缓存
	char *p_jpg = jpg_buffer;					//指向图片数据的指针
	memset(jpg_buffer,0,sizeof(jpg_buffer));	//清0
	int read_size;								//图片大小
	int send_size;								//当前发送数据的大小
	int sum = 0;								//已发送数据的总大小
	int al_send = 1024;							//每次发送的数据限制大小
	char read_size_buf[16];						//图片大小的字符串形式
	
	//读出图片数据
	if((read_size = read(jpg_fd,p_jpg,sizeof(jpg_buffer))) > 0)
	{
		printf("[OK] client read() jpg size:%d\n",read_size);
		//首先发送总长度
		memset(read_size_buf,0,sizeof(read_size_buf));
		sprintf(read_size_buf,"%d",read_size);
		if(sendto(	*client_fd, 						//socket_fd
		            read_size_buf, 						//数据内容
		            strlen(read_size_buf), 				//数据长度
		            0, 									//设为0
		            (struct sockaddr *)&server_addr, 	//目标地址
		            addr_len) == -1						//目的地址结构体长度
			)
		{
			perror("[FALSE] client sendto()");
			return NULL;
		}

		//然后发送数据，每次发送1M，直到发送read_size为止
		while(sum < read_size)
		{
			if((send_size = sendto(	*client_fd, 						//socket_fd
			                        p_jpg, 								//数据内容
			                        al_send, 							//数据长度
			                        0, 									//设为0
			                        (struct sockaddr *)&server_addr, 	//目标地址
			                        addr_len)) == -1)					//目的地址结构体长度
			{
				perror("[FALSE] client sendto()");
				return NULL;
			}
			else
			{
				//printf("[OK] client sendto() size:%d\n",send_size);
				p_jpg += send_size;
				sum += send_size;
				//检查下次发送是否超出长度
				if((sum+al_send) > read_size)
					al_send = read_size - sum;
			}
		}
		printf("[OK] client send() jpg size:%d\n",sum);

	}
	else
		perror("[FALSE] client read() bmp");

	//等待接收方回应
	int rec_size;
	char buf[128];
	memset(buf,0,sizeof(buf));
	if((rec_size = recvfrom(*client_fd, 						//socket_fd
	                        buf, 								//数据内容
	                        sizeof(buf), 						//数据长度
	                        0, 									//设为0
	                        (struct sockaddr *)&server_addr,	//发送方的地址
	                        &addr_len))== -1					//发送方地址结构体大小
	  )
	{
		perror("[FALSE] client recvfrom()");
		return NULL;
	}
	else
	{
		printf("[OK] client recvfrom() size:%d msg:%s\n",rec_size,buf);

	}

	//结束线程
	sleep(1);
	close(jpg_fd);
	pthread_exit("pthread exit!\n");
}