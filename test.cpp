#include <CwCapture.h>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <memory>

int main(int argc, char * argv[])
{
  auto cam = (CaptureDevice *) std::malloc(sizeof(CaptureDevice));

  initialize(cam, 0);

  list_options(cam);

  set_option(cam, 0);

	int width  = cam->width;
  int height = cam->height;

  alloc_buffer(cam);

  stream_on(cam);

  uint8_t * img = (uint8_t *) std::malloc(sizeof(int8_t) * cam->width * cam->height * 3);
  capture(cam, img);
  // fprintf(stdout, "address: %x\n", cam->data.imgbuf);
  // fprintf(stdout, "%d, %ld \n", cam->buffer->sequence, cam->data.usec);

  cv::Mat mat_img;
  mat_img = cv::Mat(height, width, CV_8UC3, img);
  cv::imwrite("test_image.png", mat_img);

  capture(cam, img);
  mat_img = cv::Mat(height, width, CV_8UC3, img);
  cv::imwrite("test_image2.png", mat_img);
   
  stream_off(cam);

  free_buffer(cam);
  free(cam);

  return 0;
}