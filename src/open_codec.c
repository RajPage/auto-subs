#include <libavcodec/avcodec.h>
#include <stdbool.h>
#include <stdio.h>

bool open_codec(AVCodecContext **codec_context,
                AVCodecParameters *codec_parameters) {
  // Find the decoder for the codec
  const AVCodec *codec = avcodec_find_decoder(codec_parameters->codec_id);
  if (codec == NULL) {
    printf("Could not find decoder for codec ID %d\n",
           codec_parameters->codec_id);
    return false;
  }

  // Allocate decoding context
  *codec_context = avcodec_alloc_context3(codec);
  if (!codec_context) {
    printf("Couldn't create AVCodecContext\n");
    return false;
  }

  // Fill the codec context based on the values from the supplied codec
  // parameters
  if (avcodec_parameters_to_context(*codec_context, codec_parameters) < 0) {
    printf("Couldn't initialize AVCodecContext\n");
    return false;
  }

  // Initialize the AVCodecContext to use the given AVCodec.
  if (avcodec_open2(*codec_context, codec, NULL) < 0) {
    printf("Couldn't open codec\n");
    return false;
  }

  // Available data
  // printf("Codec %s ID %d bit_rate %lld\n\n", codec->name, codec->id,
  //        codec_parameters->bit_rate);

  return true;
}