#include <libavcodec/avcodec.h>
#include <stdbool.h>
#include <stdio.h>

/// @brief Allocate frame
/// @param[out] frame
/// @return Boolean (true if successful)
bool allocate_frame(AVFrame **frame) {
  *frame = av_frame_alloc();
  if (*frame == NULL) {
    printf("Could not allocate frame\n");
    return false;
  }
  return true;
}

bool allocate_video_frame(AVFrame **output_frame, int width, int height) {
  if (!allocate_frame(output_frame)) {
    return false;
  }
  //   *output_frame = av_frame_alloc();
  //   if (*output_frame == NULL) {
  //     printf("Could not allocate frame\n");
  //     return false;
  //   }
  (*output_frame)->format = AV_PIX_FMT_RGB24;
  (*output_frame)->width = width;
  (*output_frame)->height = height;
  if (av_frame_get_buffer(*output_frame, 0) < 0) {
    printf("Could not allocate output frame data\n");
    return false;
  }
  return true;
}