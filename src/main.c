#include <GLFW/glfw3.h>
#include <libavcodec/avcodec.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool open_input_file(const char *filename,
                     AVCodecContext **input_codec_context);

int main(int argc, char **argv) {
  AVCodecContext *av_codec_context;
  GLFWwindow *window;
  if (!glfwInit()) {
    printf("Couldn't init glfw");
    return 1;
  }
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window) {
    printf("Couldn't open window");
    return 1;
  }

  // unsigned char *data = malloc(100 * 100 * 3 + 2);
  // for (int y = 0; y < 100; ++y) {
  //   for (int x = 0; x < 100; ++x) {
  //     data[y * 100 * 3 + x * 3] = 0xff;
  //     data[y * 100 * 3 + x * 3 + 1] = 0x00;
  //     data[y * 100 * 3 + x * 3 + 2] = 0x00;
  //   }
  // }
  // for (int y = 25; y < 75; ++y) {
  //   for (int x = 25; x < 75; ++x) {
  //     data[y * 100 * 3 + x * 3] = 0x00;
  //     data[y * 100 * 3 + x * 3 + 1] = 0x00;
  //     data[y * 100 * 3 + x * 3 + 2] = 0xff;
  //   }
  // }

  int frame_width, frame_height;
  unsigned char *frame_data;
  if (!open_input_file("/Users/Raj/code/sides/auto-subs/hush.mp4",
                       &av_codec_context)) {
    printf("Couldn't load");
    return 1;
  } else {
    printf("Loaded the video");
  }

  // glfwMakeContextCurrent(window);
  // // Generate a Texture
  // GLuint tex_handle;
  // glGenTextures(1, &tex_handle);
  // // Bind the texture to GL_TEXTURE_2D
  // glBindTexture(GL_TEXTURE_2D, tex_handle);
  // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  // // Load data to GL_TEXTURE_2D
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0,
  // GL_RGB,
  //              GL_UNSIGNED_BYTE, frame_data);

  // while (!glfwWindowShouldClose(window)) {
  //   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //   // glDrawPixels(100, 100, GL_RGB, GL_UNSIGNED_BYTE, data);

  //   // Set up orthographic projection
  //   // Ensure 1 to 1 projection with the window
  //   int window_width, window_height;
  //   glfwGetFramebufferSize(window, &window_width, &window_height);
  //   glMatrixMode(GL_PROJECTION);
  //   glLoadIdentity();
  //   glOrtho(0, window_width, 0, window_height, -1, 1);
  //   glMatrixMode(GL_MODELVIEW);

  //   glEnable(GL_TEXTURE_2D);
  //   glBindTexture(GL_TEXTURE_2D, tex_handle);
  //   glBegin(GL_QUADS);
  //   glTexCoord2d(0, 0);
  //   glVertex2i(200, 200);
  //   glTexCoord2d(1, 0);
  //   glVertex2i(200 + frame_width, 200);
  //   glTexCoord2d(1, 1);
  //   glVertex2i(200 + frame_width, 200 + frame_height);
  //   glTexCoord2d(0, 1);
  //   glVertex2i(200, 200 + frame_height);
  //   glEnd();
  //   glDisable(GL_TEXTURE_2D);

  //   glfwSwapBuffers(window);

  //   glfwWaitEvents();
  // }
  return 0;
}