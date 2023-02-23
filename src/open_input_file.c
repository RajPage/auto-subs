#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <stdbool.h>
#include <stdio.h>

bool open_input_file(const char *filename,
                     AVCodecContext **input_codec_context) {
  AVCodecContext *av_codec_ctx;
  AVCodecParameters *av_codec_params;
  AVCodec *av_codec;
  AVStream *stream;
  int audio_stream_index = -1;

  // Open the input file to read from it
  AVFormatContext *av_format_ctx = avformat_alloc_context();
  if (!av_format_ctx) {
    printf("Couldn't create AVFormatContext\n");
    return false;
  }
  if (avformat_open_input(&av_format_ctx, filename, NULL, NULL) < 0) {
    printf("Couldn't open file\n");
    return false;
  }

  // Get information on the input file (number of streams, etc)
  if (avformat_find_stream_info(av_format_ctx, NULL) < 0) {
    printf("Couldn't find stream info\n");
    avformat_close_input(av_format_ctx);
    return false;
  }

  // Make sure there is at least one audio stream in the file.
  for (int i = 0; i < av_format_ctx->nb_streams; ++i) {
    av_codec_params = av_format_ctx->streams[i]->codecpar;

    if (av_codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
      audio_stream_index = i;
      // @NOTE: Supporting only one audio stream for now.
      break;
    }
  }
  if (audio_stream_index == -1) {
    printf("No audio stream inside file\n");
    avformat_close_input(av_format_ctx);
    return false;
  }
  stream = av_format_ctx->streams[audio_stream_index];

  // Find a decoder for the audio stream
  av_codec = avcodec_find_decoder(stream->codecpar->codec_id);
  if (!av_codec) {
    printf("Could not find input codec\n");
    avformat_close_input(av_format_ctx);
    return false;
  }

  // Allocate decoding context
  av_codec_ctx = avcodec_alloc_context3(av_codec);
  if (!av_codec_ctx) {
    printf("Couldn't create AVCodecContext\n");
    avformat_close_input(av_format_ctx);
    return false;
  }

  // Initialize stream parameters
  if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
    printf("Couldn't initialize AVCodecContext\n");
    avformat_close_input(av_format_ctx);
    avcodec_free_context(&av_codec_ctx);
    return false;
  }

  // Open Decoder
  if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
    printf("Couldn't open codec\n");
    avformat_close_input(av_format_ctx);
    avcodec_free_context(&av_codec_ctx);
    return false;
  }

  // Set packet timebase
  av_codec_ctx->pkt_timebase = stream->time_base;

  // Save decoder context
  *input_codec_context = av_codec_ctx;
  return true;
}