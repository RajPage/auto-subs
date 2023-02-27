#include <libavformat/avformat.h>
#include <stdbool.h>
#include <stdio.h>

/// @brief Find and returns the first stream of the given type
/// @param      input_context   Format context of opened file
/// @param      type            "audio" | "video"
/// @param[out] stream_index    Index of the stream
/// @return Boolean (false if not found)
bool find_stream(AVFormatContext *input_context, const char *type,
                 int *stream_index) {
  for (int i = 0; i < input_context->nb_streams; i++) {

    // Available Info
    // AVStream *stream = input_context->streams[i];
    // printf("AVStream->time_base before open coded %d/%d\t",
    //        stream->time_base.num, stream->time_base.den);
    // printf("AVStream->r_frame_rate before open coded %d/%d\t",
    //        stream->r_frame_rate.num, stream->r_frame_rate.den);
    // printf("AVStream->start_time %" PRId64, stream->start_time);
    // printf("\t");
    // printf("AVStream->duration %" PRId64, stream->duration);
    // printf("\n");

    if (type == "video" &&
        input_context->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
      // @TODO: Find available data here
      *stream_index = i;
      return true;
    } else if (type == "audio" &&
               input_context->streams[i]->codecpar->codec_type ==
                   AVMEDIA_TYPE_AUDIO) {

      // Available Info
      // printf("Audio Codec: %d channels, sample rate %d\n",
      //        stream->codecpar->channels, stream->codecpar->sample_rate);

      *stream_index = i;
      return true;
    }

    printf("Could not find any %s stream in the input file\n", type);
    return false;
  }
}