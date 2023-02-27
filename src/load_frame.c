#include <inttypes.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <stdbool.h>
#include <stdlib.h>

bool load_frame(const char *filename, int *width, int *height,
                unsigned char **data) {

  AVCodecParameters *av_codec_params;
  AVCodec *av_codec;
  int video_stream_index = -1;
  int audio_stream_index = -1;

  AVFormatContext *av_format_ctx = avformat_alloc_context();
  if (!av_format_ctx) {
    printf("Couldn't alloc AVFormatContext");
    return false;
  }
  if (avformat_open_input(&av_format_ctx, filename, NULL, NULL) != 0) {
    printf("Couldn't open file");
    return false;
  }

    for (int i = 0; i < av_format_ctx->nb_streams; ++i) {
    av_codec_params = av_format_ctx->streams[i]->codecpar;
    av_codec = avcodec_find_decoder(av_codec_params->codec_id);

    if (!av_codec) {
      continue;
    }
    if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_index = i;
      //   break;
    }
    if (av_codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
      audio_stream_index = i;
    }
  }

  if (video_stream_index == -1) {
    printf("No video stream inside file");
    return false;
  }

  AVCodecContext *av_codec_ctx = avcodec_alloc_context3(av_codec);
  if (!av_codec_ctx) {
    printf("Couldn't create AVCodecContext\n");
    return false;
  }
  if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
    printf("Couldn't initialize AVCodecContext\n");
    return false;
  }
  if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
    printf("Couldn't open codec\n");
    return false;
  }

  avformat_close_input(&av_format_ctx);
  avformat_free_context(av_format_ctx);
  avcodec_free_context(&av_codec_ctx);

  return false;
}