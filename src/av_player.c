#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdbool.h>
#include <stdio.h>

bool open_input_file(AVFormatContext **input_format_context,
                     const char *file_name);
bool find_stream(AVFormatContext *input_context, int *stream_index,
                 const char *type);
bool open_codec(AVCodecContext **codec_context,
                AVCodecParameters *codec_parameters);
bool allocate_frame(AVFrame **frame);
bool allocate_video_frame(AVFrame **output_frame, int width, int height);

int main(int argc, char *argv[]) {
  // Check for correct number of args
  if (argc < 2) {
    printf("Please enter the file name");
    return EXIT_FAILURE;
  }

  //   // FFmpeg init
  //   av_register_all();

  // Open the file
  AVFormatContext *input_context = NULL;
  if (!open_input_file(&input_context, argv[1])) {
    return EXIT_FAILURE;
  }
  printf("File opened successfully\n");

  // Find the first video stream
  int video_stream_index = -1;
  if (!find_stream(input_context, &video_stream_index, "video")) {
    return EXIT_FAILURE;
  }
  printf("Video stream found\n");

  // Open the codec
  AVCodecParameters *video_codec_parameters =
      input_context->streams[video_stream_index]->codecpar;
  AVCodecContext *video_codec_context = NULL;
  if (!open_codec(&video_codec_context, video_codec_parameters)) {
    return EXIT_FAILURE;
  }
  printf("Codec opened\n");

  // Allocate frame
  AVFrame *video_frame = NULL;
  if (!allocate_frame(&video_frame)) {
    return EXIT_FAILURE;
  }
  printf("Reached End successfully");
  return EXIT_SUCCESS;
}