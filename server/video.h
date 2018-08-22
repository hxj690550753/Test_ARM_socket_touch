#ifndef VIDEO_H_
#define VIDEO_H_

#define CAMERA_PATH 	"/dev/video7"

/**
  * @brief	初始化摄像头，并让摄像头开始工作
  * @param	None
  * @retval	None
  */
void video_init(void);

/**
  * @brief	让摄像头停止工作，并关闭摄像头
  * @param	None
  * @retval	None
  */
void video_close(void);

#endif
