#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#endif

#include "gif_load.h"

#define FAIL \
    {\
    perror("Usage: ascii_converter <image_path> <output_height> <output_width>");\
    perror("Usage: ascii_converter <image_path> <output_width>");\
    LOG("Failed in function %s", __FUNCTION__);\
    exit(1);\
    }

#ifdef DEBUG
#define LOG(_fmt, ...) printf("[DEBUG] [" __FILE__ ":%d] " _fmt "\n", __LINE__, ##__VA_ARGS__);
#else
#define LOG(...)
#endif

#define HEIGHT_RATIO 0.5f
#define CALCULATE_GLOB \
    {\
    if(glob.out_h == -1) {\
      glob.out_h = floorf(((float) glob.h / glob.w) * glob.out_w * HEIGHT_RATIO);\
    }\
    glob.ratio_w = glob.w / glob.out_w;\
    glob.ratio_h = glob.h / glob.out_h;\
    }

// source: https://paulbourke.net/dataformats/asciiart/
#if 0
static char map[] = " .\'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
#else
static char map[] = " .:-=+*#%@";
#endif

struct {
  int out_w, out_h;
  int ratio_h, ratio_w;
  int w, h, chs;
  _Bool gif_done;
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
  int index = floor(darkness * (sizeof(map) - 1));

  return map[index];
}

void convert_frame(unsigned char *frame, char *out) {
  memset(out, '\0', glob.out_h * (glob.out_w+1) * sizeof(char));
  for(int hh=0;hh < glob.out_h;hh++) {
    for(int ww=0;ww < glob.out_w;ww++) {
      out[hh*(glob.out_w+1) + ww] = sample_value(frame, ww, hh);
    }
  }
}

void clear_screen(void) {
  printf("\x1b[%dA", glob.out_h);
  printf("\x1b[%dD", glob.out_w);
}

void display(char *out) {
  for(int hh=0;hh < glob.out_h;hh++) puts(&out[hh*(glob.out_w+1)]);
}

void gif_callback(void *, struct GIF_WHDR *frame) {
  glob.w = frame->xdim;
  glob.h = frame->ydim;
  CALCULATE_GLOB
  glob.chs = 3;

  char *out = malloc((glob.out_w+1)*glob.out_h*sizeof(char));
  unsigned char *fr_data = malloc(glob.w * glob.h * glob.chs);
  for(int hh=0;hh < glob.h;hh++) {
    for(int ww=0;ww < glob.w;ww++) {
      int index = hh * glob.w * glob.chs + ww * glob.chs;

      fr_data[index + 0] = frame->cpal[frame->bptr[hh*glob.w+ww]].R;
      fr_data[index + 1] = frame->cpal[frame->bptr[hh*glob.w+ww]].G;
      fr_data[index + 2] = frame->cpal[frame->bptr[hh*glob.w+ww]].B;
    }
  }

  convert_frame(fr_data, out);
  display(out);
  clear_screen();
  free(out);
  free(fr_data);

  // gif time units (1 unit = 10 msec)
  thrd_sleep(&(struct timespec){ .tv_nsec = frame->time * 1e7 }, NULL);
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

  if(strstr(argv[1], ".gif")) { // handle gifs
    LOG("THIS IS A GIF");

    FILE *gif_f = fopen(argv[1], "rb");
    if(gif_f == NULL) FAIL
    fseek(gif_f, 0, SEEK_END); // find end
    size_t size = ftell(gif_f); // get offset from start (file size)
    fseek(gif_f, 0, SEEK_SET); // return to start
    
    void *buff = malloc(size * sizeof(char));
    if(fread(buff, sizeof(char), size, gif_f) == 0) FAIL
    fclose(gif_f);

    // handle actual rendering in gif_callback
    GIF_Load(buff, size, gif_callback, NULL, NULL, 0);

    #ifdef GIF_LOOP
    while(true)
      GIF_Load(buff, size, gif_callback, NULL, NULL, 0);
    #endif

    free(buff);
  }
  else { // handle normal images
    // open image
    unsigned char *img = stbi_load(argv[1], &glob.w, &glob.h, &glob.chs, 0);
    if(img == NULL) FAIL
    LOG("Opened image: w=%d, h=%d, chs=%d", glob.w, glob.h, glob.chs);

    CALCULATE_GLOB
    // convert frame
    char *out = malloc(glob.out_h * (glob.out_w + 1));
    convert_frame(img, out);
    // output result
    display(out);
    free(out);
    stbi_image_free(img);
  }

  return 0;
}
