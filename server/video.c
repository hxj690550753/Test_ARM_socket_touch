#include "video.h"
#include "api_v4l2.h"

/*
typedef struct Frame_Buffer
{
	char buf[1843200];
	int length;
	
}FrameBuffer;
*/

/**
  * @brief	初始化摄像头，并让摄像头开始工作
  * @param	None
  * @retval	None
  */
void video_init(void)
{
	//初始化
	linux_v4l2_device_init(CAMERA_PATH);
	//开始工作
	linux_v4l2_start_capturing();
}

/**
  * @brief	让摄像头停止工作，并关闭摄像头
  * @param	None
  * @retval	None
  */
void video_close(void)
{
	//停止工作
	linux_v4l2_stop_capturing();
	//关闭video
	linux_v4l2_device_uinit();
}





