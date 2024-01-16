//
// Author: Chanwoo Lee (leechanwoo25@gmail.com)
//
#include <v4l2utils.h>

#define UYVY_BYTES_PER_PIXEL 2U


struct v4l2_fmtdesc * get_v4l2_fmtdesc(const uint32_t v_idx,
                                       const uint32_t f_idx,
                                       const uint8_t verbose)
{
  char * video_node = (char *) malloc(sizeof(char) * 16);
  sprintf(video_node, "/dev/video%d", v_idx);
  int fd = open(video_node, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Node %s: %s\n", video_node, strerror(errno));
    goto exit_video_node;
  }

	struct v4l2_fmtdesc * fmt =
    (struct v4l2_fmtdesc *) malloc(sizeof(struct v4l2_format));
  if (fmt < 0) {
    fprintf(stderr, "%s(): memory allocation failed(fmt)%s\n", __func__, strerror(errno));
    goto exit_fmt;
  }

	fmt->index = f_idx;
	fmt->type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_ENUM_FMT, fmt) < 0) {
    fprintf(stderr, "ioctl VIDIOC_ENUM_FMT is failed: %s\n", strerror(errno));
    goto exit_fmt;
  }
  else
  {
    if (verbose)
    {
      fprintf(stdout, "(%s)(capture-%02d) supported format -> %s\n",
                       video_node, f_idx, fmt->description);
    }
    close(fd);
    free(video_node);
    return fmt;
  }

exit_fmt:
  free(fmt);
  close(fd);
exit_video_node:
  free(video_node);
  return NULL;
}

struct v4l2_fmtdesc * get_v4l2_fmtdesc_of(const int fd, const uint32_t f_idx)
{
	struct v4l2_fmtdesc * fmt =
    (struct v4l2_fmtdesc *) malloc(sizeof(struct v4l2_format));
  if (fmt < 0) {
    fprintf(stderr, "%s(): memory allocation failed(fmt)%s\n", __func__, strerror(errno));
    free(fmt);
    return NULL;
  }

	fmt->index = f_idx;
	fmt->type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_ENUM_FMT, fmt) < 0) {
    fprintf(stderr, "ioctl VIDIOC_ENUM_FMT is failed: %s\n", strerror(errno));
    free(fmt);
    return NULL;
  }
  else
  {
    return fmt;
  }
}

struct v4l2_frmsizeenum * get_v4l2_frmsizeenum(const uint32_t v_idx,
                                               const uint32_t f_idx,
                                               const uint32_t pixel_format,
                                               const uint8_t verbose)
{
  char * video_node = (char *) malloc(sizeof(char) * 16);
  sprintf(video_node, "/dev/video%d", v_idx);
  int fd = open(video_node, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Node %s: %s\n", video_node, strerror(errno));
    goto exit_video_node;
  }

	struct v4l2_frmsizeenum * frmsize =
    (struct v4l2_frmsizeenum *) malloc (sizeof(struct v4l2_frmsizeenum));
  if (frmsize < 0) {
    fprintf(stderr, "%s(): memory allocation failed(frmsizeenum)%s\n", __func__, strerror(errno));
    goto exit_frmsize;
  }

	frmsize->pixel_format = pixel_format;
	frmsize->index        = f_idx;
	if (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, (frmsize)) < 0) {
    fprintf(stderr, "ioctl VIDIOC_ENUM_FRAMESIZES is failed: %s\n", strerror(errno));
    goto exit_frmsize;
  }
  else
  {
    assert (frmsize->type == V4L2_FRMSIZE_TYPE_DISCRETE);
    if (verbose)
    {
      fprintf(stdout, "(%s)(format-%d) frame size: %d x %d, \n",
                       video_node, f_idx, frmsize->discrete.width, frmsize->discrete.height);
    }
    close(fd);
    free(video_node);
    return frmsize;
  }

exit_frmsize:
  free(frmsize);
  close(fd);
exit_video_node:
  free(video_node);
  return NULL;
}

struct v4l2_frmsizeenum * get_v4l2_frmsizeenum_of(const int fd,
                                                   const uint32_t f_idx,
                                                   const uint32_t pixel_format)
{
	struct v4l2_frmsizeenum * frmsize = (struct v4l2_frmsizeenum *) malloc (sizeof(struct v4l2_frmsizeenum));
  if (frmsize < 0) {
    fprintf(stderr, "%s(): memory allocation failed(frmsizeenum)%s\n", __func__, strerror(errno));
    return NULL;
  }

	frmsize->pixel_format = pixel_format;
	frmsize->index        = f_idx;
	if (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, (frmsize)) < 0) {
    // fprintf(stderr, "ioctl VIDIOC_ENUM_FRAMESIZES is failed: %s\n", strerror(errno));
    free(frmsize);
    return NULL;
  }
  else
  {
    assert (frmsize->type == V4L2_FRMSIZE_TYPE_DISCRETE);
    return frmsize;
  }
}

int set_v4l2_format_of(const int fd,
                       const uint32_t width,
                       const uint32_t height,
                       const uint32_t pixel_format,
                       const uint8_t verbose)
{
	struct v4l2_format fmt;
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (ioctl(fd, VIDIOC_G_FMT, &fmt) < 0) {
    fprintf(stderr, "ioctl VIDIO_G_FMT is failed: %s\n", strerror(errno));
		return -1;
	}

  // assert (fmt.fmt.pix.pixelformat == v4l2_fourcc('U', 'Y', 'V', 'Y'));
	fmt.fmt.pix.pixelformat = pixel_format;
	fmt.fmt.pix.width       = width;
	fmt.fmt.pix.height      = height;
	fmt.fmt.pix.sizeimage   = width * height * UYVY_BYTES_PER_PIXEL;

  if (verbose)
  {
    fprintf(stdout, "%s(): try [format: %d, width, %d, height: %d]\n",
                    __func__, fmt.fmt.pix.pixelformat, width, height);
  }

	if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
		fprintf(stderr, "ioctl VIDIOC_S_FMT is failed: %s\n", strerror(errno));
		return -1;
	}

  struct v4l2_format chk_fmt;
	memset(&chk_fmt, 0, sizeof(struct v4l2_format));
	chk_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_G_FMT, &chk_fmt) < 0) {
    fprintf(stderr, "ioctl VIDIO_G_FMT is failed: %s\n", strerror(errno));
		return -1;
	}

  if ((fmt.type == chk_fmt.type)
       && (fmt.fmt.pix.pixelformat == chk_fmt.fmt.pix.pixelformat)
       && (fmt.fmt.pix.width == chk_fmt.fmt.pix.width)
       && (fmt.fmt.pix.height == chk_fmt.fmt.pix.height))
  {
    if (verbose)
    {
      fprintf(stdout, "%s(): device setup complete. [%d x %d]\n", __func__, width, height);
    }
    return 0;
  }
  else
  {
    fprintf(stderr, "%s(): device setup complete, but different framesize. [%d x %d]\n",
                    __func__, width, height);
    return 1;
  }
}

// // in -- zero based video num
// // out availbble capture types, sizes, format
void list_capture_resolutions_of_video(const int v_idx)
{
  char * video_node = (char *) malloc(sizeof(char) * 16);
  sprintf(video_node, "/dev/video%d", v_idx);
  int fd = open(video_node, O_RDWR);
  if (fd < 0) {
    fprintf(stderr, "Node %s: %s\n", video_node, strerror(errno));
    goto exit_video_node;
  }

	struct v4l2_fmtdesc * fmt = get_v4l2_fmtdesc(v_idx, 0, 0);
  if (!fmt) {
    fprintf(stderr, "%s(): Fail to read fmtdesc.\n", __func__);
    goto exit_fmt;
  }

	struct v4l2_frmsizeenum * frmsize = NULL;
  // Assumption: All format index has same pixel_format.
  fprintf(stdout, "List of available frame-size.\n");
  for (uint8_t f_idx = 0; ; ++f_idx)
  {
    frmsize = get_v4l2_frmsizeenum(v_idx, f_idx, fmt->pixelformat, 1);
    if (!frmsize) {
      if (f_idx == 0)
        fprintf(stderr, "%s(): No available frame size.\n", __func__);
      break;
    }
	  free(frmsize);
  }
  fprintf(stdout, "List end.\n");

exit_fmt:
	free(fmt);
	close(fd);
exit_video_node:
  free(video_node);
}

int v4l2_capture_start(int fd, int buf_type) {
	if (ioctl(fd, VIDIOC_STREAMON, &buf_type) < 0) {
    fprintf(stderr, "ioctl VIDIOC_STREAMON: %s\n", strerror(errno));
    return -1;
	}
  else
  	return 0;
}

int v4l2_capture_stop(int fd, int buf_type) {
	if (ioctl(fd, VIDIOC_STREAMOFF, &buf_type) < 0) {
    fprintf(stderr, "ioctl VIDIOC_STREAMOFF: %s\n", strerror(errno));
    return -1;
	}
  else
  	return 0;
}


int set_v4l2_reqbuf_userptr(int fd,
                            unsigned int max_count,
                            unsigned int buf_type,
                            unsigned int mam_type)
{
	struct v4l2_requestbuffers reqbuf;
	long sz = sysconf(_SC_PAGESIZE);
	memset(&reqbuf, 0, sizeof(struct v4l2_requestbuffers));

	reqbuf.count  = max_count;
	reqbuf.type   = buf_type;
	reqbuf.memory = mam_type;
	if (ioctl(fd, VIDIOC_REQBUFS, &reqbuf) < 0) {
    fprintf(stderr, "ioctl VIDIOC_REQBUFS: %s\n", strerror(errno));
		return -1;
	}
  return 0;
}

int allocate_v4l2_buffer(int fd, struct v4l2_buffer ** allocated_buffer, uint32_t img_bytes_size)
{
  long sz = sysconf(_SC_PAGESIZE);

  *allocated_buffer = (struct v4l2_buffer *) malloc (sizeof(struct v4l2_buffer));
  struct v4l2_buffer * buffer = (*allocated_buffer);
  if (buffer < 0) {
    fprintf(stderr, "v4l2_buffer memory allocation failed. %s\n", strerror(errno));
    return -1;
  }

  memset(buffer, 0, sizeof(struct v4l2_buffer));
  buffer->index = 0;
  buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buffer->length = img_bytes_size;

  void * void_ptr = aligned_alloc(sz, img_bytes_size);
  if (void_ptr == NULL) {
    fprintf(stderr, "aligned_alloc failed: %s\n", strerror(errno));
    free(buffer);
    return -1;
  }

  buffer->m.userptr = (unsigned long) void_ptr;
  buffer->memory = V4L2_MEMORY_USERPTR;
  if (ioctl(fd, VIDIOC_QBUF, buffer) < 0) {
    fprintf(stderr, "ioctl VIDIOC_QBUF: %s\n", strerror(errno));
    free(buffer);
    return -1;
  }

	return 0;
}

int v4l2_capture_dq_v4l2_buffer(int fd, struct v4l2_buffer * buffer)
{
  // memset(buffer, 0, sizeof(struct v4l2_buffer));
	buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_DQBUF, buffer) < 0) {
    fprintf(stderr, "ioctl VIDIOC_DQBUF: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

int v4l2_capture_q_v4l2_buffer(int fd, struct v4l2_buffer * buffer)
{
	buffer->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_QBUF, buffer) < 0) {
    fprintf(stderr, "ioctl VIDIOC_QBUF: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

uint8_t * get_frame_virt(struct v4l2_buffer * buffer) {
	return (uint8_t *)(void *)buffer->m.userptr;
}

void clean_buffer(struct v4l2_buffer * buffer) {
  memset(buffer, 0, sizeof(struct v4l2_buffer));
}

int yuv_rgb888_conversion(uint8_t * yuyv_buffer,
                          uint8_t * rgb_buffer,
                          int width_act,
                          int height_act)
{
  int i = 0, j = 0;
  int temp;
  int yuvcount = 0;
  int rgbcount = 0;
  int width = width_act;
  int height = height_act;
  float u_val, v_val, y1_val, y2_val;

  for (i = 0; i < height; i++)
  {
    for (j = 0; j < width; j += 2)
    {
      u_val = (float)yuyv_buffer[yuvcount++];
      y1_val = (float)yuyv_buffer[yuvcount++];
      v_val = (float)yuyv_buffer[yuvcount++];
      y2_val = (float)yuyv_buffer[yuvcount++];

      u_val = u_val - 128;
      v_val = v_val - 128;

      temp = (int)(y1_val + (1.770 * u_val));
      rgb_buffer[(((height - 1) - i) * width * 3) + j * 3 + 0] =
          (temp > 255) ? 255 : ((temp < 0) ? 0 : (unsigned char)temp);
      rgbcount++;

      temp = (int)(y1_val - (0.344 * u_val) - (0.714 * v_val));
      rgb_buffer[(((height - 1) - i) * width * 3) + j * 3 + 1] =
          (temp > 255) ? 255 : ((temp < 0) ? 0 : (unsigned char)temp);
      rgbcount++;

      temp = (int)(y1_val + (1.403 * v_val));
      rgb_buffer[(((height - 1) - i) * width * 3) + j * 3 + 2] =
          (temp > 255) ? 255 : ((temp < 0) ? 0 : (unsigned char)temp);
      rgbcount++;

      temp = (int)(y2_val + (1.770 * u_val));
      rgb_buffer[(((height - 1) - i) * width * 3) + j * 3 + 3] =
          (temp > 255) ? 255 : ((temp < 0) ? 0 : (unsigned char)temp);
      rgbcount++;

      temp = (int)(y2_val - (0.344 * u_val) - (0.714 * v_val));
      rgb_buffer[(((height - 1) - i) * width * 3) + j * 3 + 4] =
          (temp > 255) ? 255 : ((temp < 0) ? 0 : (unsigned char)temp);
      rgbcount++;

      temp = (int)(y2_val + (1.403 * v_val));
      rgb_buffer[(((height - 1) - i) * width * 3) + j * 3 + 5] =
          (temp > 255) ? 255 : ((temp < 0) ? 0 : (unsigned char)temp);
      rgbcount++;
    }
  }
  return 0;
}

/*
uffer timestamp:12769385133
queue-> bytesused:1843200
, flags:8194
, field:1
, sequence:0
, length:1843200
dequeue -> bytesused:1843200
, flags:8192
, field:1
, sequence:116
, length:1843200
buffer timestamp:12769393466
queue-> bytesused:1843200
, flags:8194
, field:1
, sequence:0
, length:1843200
dequeue -> bytesused:1843200
, flags:8192
, field:1
, sequence:117
, length:1843200
buffer timestamp:12769410132
queue-> bytesused:1843200
, flags:8194
, field:1
, sequence:0
, length:1843200
*/
