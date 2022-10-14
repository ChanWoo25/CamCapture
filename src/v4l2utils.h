#ifndef V4L2_UTILS_H__
#define V4L2_UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <assert.h>

struct v4l2_fmtdesc * get_v4l2_fmtdesc(const uint32_t v_idx,  
                                       const uint32_t f_idx, 
                                       const uint8_t verbose);

struct v4l2_fmtdesc * get_v4l2_fmtdesc_of(const int fd, 
                                           const uint32_t f_idx);

struct v4l2_frmsizeenum * get_v4l2_frmsizeenum(const uint32_t v_idx, 
                                               const uint32_t f_idx,
                                               const uint32_t pixel_format, 
                                               const uint8_t verbose);

struct v4l2_frmsizeenum * get_v4l2_frmsizeenum_of(const int fd, 
                                                   const uint32_t f_idx, 
                                                   const uint32_t pixel_format);
 
int set_v4l2_format_of(const int fd,
                       const uint32_t width,
                       const uint32_t height,
                       const uint32_t pixel_format,
                       const uint8_t verbose);

void list_capture_resolutions_of_video(const int v_idx);

// Open a camera device and return fd. If failed, return -1;
int open_device(const uint32_t v_idx);

// close device file according to given file descriptor. 
int close_device(int fd);

int v4l2_capture_start(int fd, int buf_type);

int v4l2_capture_stop(int fd, int buf_type);

int set_v4l2_reqbuf_userptr(int fd, 
                            unsigned int max_count, 
                            unsigned int buf_type, 
                            unsigned int mam_type);

int allocate_v4l2_buffer(int fd, struct v4l2_buffer ** allocated_buffer, uint32_t img_bytes_size);

int v4l2_capture_dq_v4l2_buffer(int fd, struct v4l2_buffer * buffer);

int v4l2_capture_q_v4l2_buffer(int fd, struct v4l2_buffer * buffer);

uint8_t * get_frame_virt(struct v4l2_buffer * buffer);

int get_camera_frames(int fd, struct v4l2_buffer * buffer, uint64_t * usec);

int yuv_rgb888_conversion(uint8_t * yuyv_buffer, 
                          uint8_t * rgb_buffer, 
                          int width_act, 
                          int height_act);
                          
// // Unused function. Some camera device may use V4L2_CID_FRAME_SYNC Control ID for frame sync.
// // Just remember that CID can be controllable with below function impl. (ioctl VIDIOC_S_CTRL)
// #define V4L2_CID_FRAME_SYNC (V4L2_CID_AUTO_FOCUS_RANGE+11)
// int v4l2_capture_set_sync(int fd, int value)
// {
// 	struct v4l2_control ctrl;
// 	memset(&ctrl, 0, sizeof(struct v4l2_control));

// 	ctrl.id    = V4L2_CID_FRAME_SYNC;
// 	ctrl.value = value;
// 	if (ioctl(fd, VIDIOC_S_CTRL, &ctrl) < 0) {
// 		fprintf(stderr, "unable to set sync: %s\n", strerror(errno));
// 		return -1;
// 	}
// 	return 0;
// }

#endif