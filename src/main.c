#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#endif

#define FAIL \
    {\
    perror("Usage: ascii_converter <image_path> <output_height> <output_width>");\
    perror("Usage: ascii_converter <image_path> <output_width>");\
    exit(1);\
    }

#ifdef DEBUG
#define LOG(_fmt, ...) printf("[DEBUG] [" __FILE__ ":%d] " _fmt "\n", __LINE__, __VA_ARGS__);
#else
#define LOG(...)
#endif

#define HEIGHT_RATIO 0.5f

#if 0
static char map[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
#else
static char map[] = " .:-=+*#%@";
#endif

char sample_value(unsigned char *img, int ratio_w, int ratio_h, int ww, int hh, int chs, int width) {
  int avg = 0;

  for(int i=0;i < ratio_h;i++) {
    for(int j=0;j < ratio_w;j++) {
      int curr_w = ww * ratio_w + j;
      int curr_h = hh * ratio_h + i;

      unsigned char *curr_pixel = img + curr_h*width*chs + curr_w*chs;
      int curr = 0;
      for(int chan=0;chan < chs;chan++) curr += curr_pixel[chan];
      curr /= chs;

      avg += curr;
    }
  }
  avg /= ratio_w * ratio_h;

  float darkness = (float) avg / 255.0f;
  int index = floor(darkness * sizeof(map));

  return map[index];
}

int main(int argc, char **argv) {
  if(argc != 4 && argc != 3) FAIL

  int out_h, out_w;
  if (argc == 4) {
    out_h = atoi(argv[2]);
    out_w = atoi(argv[3]);
    if(out_h == 0 || out_w == 0) FAIL
  }
  else {
    out_w = atoi(argv[2]);
    out_h = -1;

    if(out_w == 0) FAIL
  }

  // open image
  int w, h, chs;
  unsigned char *img = stbi_load(argv[1], &w, &h, &chs, 0);
  if(img == NULL) FAIL
  LOG("Opened image: w=%d, h=%d, chs=%d", w, h, chs);

  if(out_h == -1) {
    out_h = floorf(((float) h / w) * out_w * HEIGHT_RATIO);
  }

  // convert image
  int ratio_w = w / out_w;
  int ratio_h = h / out_h;
  char out[out_h][out_w+1];
  memset(out, '\0', sizeof(out));
  for(int hh=0;hh < out_h;hh++) {
    for(int ww=0;ww < out_w;ww++) {
      out[hh][ww] = sample_value(img, ratio_w, ratio_h, ww, hh, chs, w);
    }
  }

  // output result
  for(int hh=0;hh < out_h;hh++) puts(out[hh]);
  stbi_image_free(img);
  return 0;
}
