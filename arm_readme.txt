/**简介**/
手机端用Wifi连接主控芯片，主控芯片主要运行3条线程，一条线程用于处理手机端的数据请求与回应，一条线程用于实时，显示监控画面，一条线程用于处理触摸屏数据。

/**文件列表**/
server.h		//主进程，用于初始化LCD、触摸屏和监控摄像头以及创建线程
server.c
show_bmp.h		//显示bmp图片的API
show_bmp.c
show_jpg.h		//显示jpg图片的API
show_jpg.c
touch.h			//处理触摸屏的API
touch.c
video.h			//处理摄像头的API
video.c

/**功能**/
手机端用Wifi连接主控芯片后，手机能够看到实时的监控数据，同时手机能够发送请求，用于控制主控芯片上的硬件，比如LED、BEEP等等的器件。