#include <CwCapture.h>
#include "v4l2utils.h"

int initialize(struct CaptureDevice * cam, int v_idx) {
  sprintf(cam->node_name, "/dev/video%d", v_idx);
  cam->buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
  cam->mem_type = V4L2_MEMORY_USERPTR; 
  cam->debug = 1;

  cam->v_idx = v_idx;
  cam->fd = open(cam->node_name, O_RDWR);
  if (cam->fd < 0) {
    fprintf(stderr, "Node %s: %s\n", cam->node_name, strerror(errno));
    return -1;
  }

  cam->buffer = NULL;
  cam->data.imgbuf = NULL;

  return 0;
}

int list_options(struct CaptureDevice * cam) {
	struct v4l2_fmtdesc * fmt = get_v4l2_fmtdesc_of(cam->fd, 0);
  if (fmt == NULL)
    return -1;

  // Assumption: All format index has same pixel_format.
	struct v4l2_frmsizeenum * frmsize = NULL; 
  fprintf(stdout, "\n# List of available frame-size. \n");
  for (uint8_t f_idx = 0; ; ++f_idx)
  {
    frmsize = get_v4l2_frmsizeenum_of(cam->fd, f_idx, fmt->pixelformat);
    if (frmsize == NULL)
    {
      if (f_idx == 0) {
        fprintf(stderr, "%s(): No available frame size.\n", __func__);
	      free(fmt);
        return -1;
      }
      else
        break;
    }
    else 
    {
      fprintf(stdout, "(%s)(format-%d) frame size: [%4d x %4d] \n", cam->node_name, f_idx, frmsize->discrete.width, frmsize->discrete.height);      
      free(frmsize);  
    }
  }

	free(fmt);
  return 0;
}

int set_option(struct CaptureDevice * cam, int f_idx)
{
  int flag;
  cam->f_idx = f_idx;
  
	struct v4l2_fmtdesc * fmt = get_v4l2_fmtdesc_of(cam->fd, cam->f_idx); 
  if (fmt == NULL)
    return -1;
  else {
    cam->pixel_format = fmt->pixelformat;
    free(fmt);
  }

	struct v4l2_frmsizeenum * frmsize = get_v4l2_frmsizeenum_of(cam->fd, cam->f_idx, cam->pixel_format);
  if (frmsize == NULL)
    return -1;
  else {
    cam->width  = frmsize->discrete.width;
    cam->height = frmsize->discrete.height;
    cam->img_bytes = cam->width * cam->height * UYVY_BYTES_PER_PIXEL;
    free(frmsize);
  }

  flag = set_v4l2_format_of(cam->fd, cam->width, cam->height, cam->pixel_format, 0);
  if (flag < 0)  
    return flag;
  else
    fprintf(stdout, "\n(%s) format %d is set. resolution: [%d x %d]\n", cam->node_name, cam->f_idx, cam->width, cam->height);
                       
  return 0;
}

int alloc_buffer(struct CaptureDevice * cam) {
  int flag;
     
  flag = set_v4l2_reqbuf_userptr(cam->fd, 1, cam->buf_type, cam->mem_type);
  if (flag < 0)  
    return flag;
  
  flag = allocate_v4l2_buffer(cam->fd, &cam->buffer, cam->img_bytes);
  if (flag < 0)
    return flag;
  
  return 0;
}

int free_buffer(struct CaptureDevice * cam) {
  if (cam->data.imgbuf != NULL) {
    free(cam->data.imgbuf);
    cam->data.imgbuf = NULL;
  }
  if (cam->buffer != NULL) {
    free(cam->buffer);
    cam->buffer = NULL;
  }
}

int stream_on(struct CaptureDevice * cam) {
  return v4l2_capture_start(cam->fd, cam->buf_type);
}

int stream_off(struct CaptureDevice * cam) {
  return v4l2_capture_stop(cam->fd, cam->buf_type);
}

int capture(struct CaptureDevice * cam) {
  v4l2_capture_dq_v4l2_buffer(cam->fd, cam->buffer);
  
  if (cam->data.imgbuf == NULL) {
    cam->data.imgbuf = (uint8_t *) malloc(sizeof(uint8_t) * cam->img_bytes);
  }

  memset(cam->data.imgbuf, 0, cam->img_bytes);
  memcpy(cam->data.imgbuf, (uint32_t *)(void *)cam->buffer->m.userptr, cam->img_bytes);
  cam->data.usec = 1000000U * cam->buffer->timestamp.tv_sec \
                       + 1U * cam->buffer->timestamp.tv_usec;

  v4l2_capture_q_v4l2_buffer(cam->fd, cam->buffer);
  return 0;
}

int convert(struct CaptureDevice * cam, uint8_t * img) {
  return yuv_rgb888_conversion(cam->data.imgbuf, img, cam->width, cam->height);
}
