#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

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
#define LOG(_fmt, ...) printf("[DEBUG] [" __FILE__ ":%d] " _fmt "\n", __LINE__, ##__VA_ARGS__);
#else
#define LOG(...)
#endif

#define HEIGHT_RATIO 0.5f

// source: https://paulbourke.net/dataformats/asciiart/
#if 0
static char map[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
#else
static char map[] = " .:-=+*#%@";
#endif

struct {
  int out_w, out_h;
  int ratio_h, ratio_w;
  int w, h, chs;
} glob;

char sample_value(unsigned char *img, int ww, int hh) {
  int avg = 0;

  for(int i=0;i < glob.ratio_h;i++) {
    for(int j=0;j < glob.ratio_w;j++) {
      int curr_w = ww * glob.ratio_w + j;
      int curr_h = hh * glob.ratio_h + i;

      unsigned char *curr_pixel = img + curr_h*glob.w*glob.chs + curr_w*glob.chs;
      int curr = 0;
      for(int chan=0;chan < glob.chs;chan++) curr += curr_pixel[chan];
      curr /= glob.chs;

      avg += curr;
    }
  }
  avg /= glob.ratio_w * glob.ratio_h;

  float darkness = (float) avg / 255.0f;
  int index = floor(darkness * sizeof(map));

  return map[index];
}

void convert_frame(unsigned char *frame, char out[glob.out_h][glob.out_w+1]) {
  memset(out, '\0', glob.out_h * (glob.out_w+1) * sizeof(char));
  for(int hh=0;hh < glob.out_h;hh++) {
    for(int ww=0;ww < glob.out_w;ww++) {
      out[hh][ww] = sample_value(frame, ww, hh);
    }
  }
}

void clear_screen(void) {
  printf("\x1b[%dA", glob.out_h);
  printf("\x1b[%dD", glob.out_w);
}

void display(char out[glob.out_h][glob.out_w+1]) {
  for(int hh=0;hh < glob.out_h;hh++) puts(out[hh]);
}

int main(int argc, char **argv) {
  if(argc != 4 && argc != 3) FAIL

  if (argc == 4) {
    glob.out_h = atoi(argv[2]);
    glob.out_w = atoi(argv[3]);
    if(glob.out_h == 0 || glob.out_w == 0) FAIL
  }
  else {
    glob.out_w = atoi(argv[2]);
    glob.out_h = -1;

    if(glob.out_w == 0) FAIL
  }

  // open image
  unsigned char *img = stbi_load(argv[1], &glob.w, &glob.h, &glob.chs, 0);
  if(img == NULL) FAIL
  LOG("Opened image: w=%d, h=%d, chs=%d", glob.w, glob.h, glob.chs);

  if(glob.out_h == -1) {
    glob.out_h = floorf(((float) glob.h / glob.w) * glob.out_w * HEIGHT_RATIO);
  }
  glob.ratio_w = glob.w / glob.out_w;
  glob.ratio_h = glob.h / glob.out_h;

  if(strstr(argv[1], ".gif") && 0) {
    LOG("THIS IS A GIF");
    stbi_info(argv[1], &glob.w, &glob.h, &glob.chs);

    //TODO: load gif frames and delays

    /*
    char out[glob.out_h][glob.out_w+1];
    for(int i=0;i < 10;i++) {
      clear_screen();
      convert_frame(img + glob.w*glob.h*glob.chs*i, out);
      // output result
      display(out);
      thrd_sleep(&(struct timespec){.tv_sec=1}, NULL); // sleep 1 sec
    }
    */
  }
  else {
    // convert frame
    char out[glob.out_h][glob.out_w+1];
    convert_frame(img, out);
    // output result
    display(out);
  }

  stbi_image_free(img);
  return 0;
}
