#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <stdio.h>
#include <stdlib.h>

static FILE *audio_dst_file = NULL;
static AVFrame *frame = NULL;
static AVPacket *pkt = NULL;
AVCodecContext *av_codec_ctx;
static int audio_frame_count = 0;

static int output_audio_frame(AVFrame *frame) {
  size_t unpadded_linesize =
      frame->nb_samples * av_get_bytes_per_sample(frame->format);
  printf("audio_frame n:%d nb_samples:%d pts:%s\n", audio_frame_count++,
         frame->nb_samples,
         av_ts2timestr(frame->pts, &av_codec_ctx->time_base));

  /* Write the raw audio data samples of the first plane. This works
   * fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
   * most audio decoders output planar audio, which uses a separate
   * plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
   * In other words, this code will write only the first audio channel
   * in these cases.
   * You should use libswresample or libavfilter to convert the frame
   * to packed data. */
  fwrite(frame->extended_data[0], 1, unpadded_linesize, audio_dst_file);

  return 0;
}

static int decode_packet(AVCodecContext *dec, const AVPacket *pkt) {
  int ret = 0;

  // submit the packet to the decoder
  ret = avcodec_send_packet(dec, pkt);
  if (ret < 0) {
    fprintf(stderr, "Error submitting a packet for decoding\n", );
    return ret;
  }

  // get all the available frames from the decoder
  while (ret >= 0) {
    ret = avcodec_receive_frame(dec, frame);
    if (ret < 0) {
      // those two return values are special and mean there is no output
      // frame available, but there were no errors during decoding
      if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN))
        return 0;

      fprintf(stderr, "Error during decoding\n");
      return ret;
    }

    // write the frame data to output file
    if (dec->codec->type == AVMEDIA_TYPE_AUDIO)
      ret = output_audio_frame(frame);

    av_frame_unref(frame);
    if (ret < 0)
      return ret;
  }

  return 0;
}

int open_input_file(const char *filename,
                    AVFormatContext **input_format_context,
                    AVCodecContext **input_codec_context) {
  int ret;

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

  audio_dst_file = fopen("output.aac", "wb");

  frame = av_frame_alloc();
  if (!frame) {
    fprintf(stderr, "Could not allocate frame\n");
    return -1;
  }

  pkt = av_packet_alloc();
  if (!pkt) {
    fprintf(stderr, "Could not allocate packet\n");
    return -1;
  }

  if (stream)
    printf("Demuxing audio from file '%s' into output.aac\n", filename);

  /* read frames from the file */
  while (av_read_frame(av_format_ctx, pkt) >= 0) {
    // check if the packet belongs to a stream we are interested in, otherwise
    // skip it
    if (pkt->stream_index == audio_stream_index)
      ret = decode_packet(av_codec_ctx, pkt);
    av_packet_unref(pkt);
    if (ret < 0)
      break;
  }

  // Need to play with
  // ffplay -f f32le -ac 1 -ar 44100 output.aac

  return 0;
}