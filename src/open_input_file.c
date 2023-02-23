#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <stdio.h>
#include <stdlib.h>

int open_input_file(const char *filename,
                    AVFormatContext **input_format_context,
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
    return -1;
  }
  if (avformat_open_input(&av_format_ctx, filename, NULL, NULL) < 0) {
    printf("Couldn't open file\n");
    return -1;
  }
  printf("File opened: format %s, duration %lld us, bit_rate %lld\n",
         av_format_ctx->iformat->name, av_format_ctx->duration,
         av_format_ctx->bit_rate);

  // Get information on the input file (number of streams, etc)
  if (avformat_find_stream_info(av_format_ctx, NULL) < 0) {
    printf("Couldn't find stream info\n");
    avformat_close_input(&av_format_ctx);
    return -1;
  }

  // Make sure there is at least one audio stream in the file.
  for (int i = 0; i < av_format_ctx->nb_streams; ++i) {
    av_codec_params = av_format_ctx->streams[i]->codecpar;
    printf("AVStream->time_base before open coded %d/%d\t",
           av_format_ctx->streams[i]->time_base.num,
           av_format_ctx->streams[i]->time_base.den);
    printf("AVStream->r_frame_rate before open coded %d/%d\t",
           av_format_ctx->streams[i]->r_frame_rate.num,
           av_format_ctx->streams[i]->r_frame_rate.den);
    printf("AVStream->start_time %" PRId64,
           av_format_ctx->streams[i]->start_time);
    printf("\t");
    printf("AVStream->duration %" PRId64, av_format_ctx->streams[i]->duration);
    printf("\n");
    if (av_codec_params->codec_type == AVMEDIA_TYPE_AUDIO) {
      audio_stream_index = i;
      printf("Audio Codec: %d channels, sample rate %d\n",
             av_codec_params->channels, av_codec_params->sample_rate);
      break;
    }
  }
  if (audio_stream_index == -1) {
    printf("File %s does not have an audio stream\n", filename);
    avformat_close_input(&av_format_ctx);
    return -1;
  }
  stream = av_format_ctx->streams[audio_stream_index];
  printf("Stream selected\n");

  // Find a decoder for the audio stream
  av_codec = avcodec_find_decoder(stream->codecpar->codec_id);
  if (!av_codec) {
    printf("Could not find input codec\n");
    avformat_close_input(&av_format_ctx);
    return -1;
  }

  // Allocate decoding context
  av_codec_ctx = avcodec_alloc_context3(av_codec);
  if (!av_codec_ctx) {
    printf("Couldn't create AVCodecContext\n");
    avformat_close_input(&av_format_ctx);
    return -1;
  }

  // Fill the codec context based on the values from the supplied codec
  // parameters
  if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
    printf("Couldn't initialize AVCodecContext\n");
    avformat_close_input(&av_format_ctx);
    avcodec_free_context(&av_codec_ctx);
    return -1;
  }

  // Initialize the AVCodecContext to use the given AVCodec.
  if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
    printf("Couldn't open codec\n");
    avformat_close_input(&av_format_ctx);
    avcodec_free_context(&av_codec_ctx);
    return -1;
  }
  printf("\tCodec %s ID %d bit_rate %lld\n\n", av_codec->name, av_codec->id,
         stream->codecpar->bit_rate);

  // Set packet timebase
  av_codec_ctx->pkt_timebase = stream->time_base;

  // Save decoder context
  *input_codec_context = av_codec_ctx;
  *input_format_context = av_format_ctx;

  return 0;
}