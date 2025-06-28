#pragma once

#include <math.h>

struct GLOB {
  int out_w, out_h;
  int ratio_h, ratio_w;
  int w, h, chs;
};

extern struct GLOB glob;

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
// calculates some global ratios
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
static const char map[] = " .\'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
#else
static const char map[] = " .:-=+*#%@";
#endif

char sample_value(unsigned char *img, int ww, int hh);
void convert_frame(unsigned char *frame, char *out);
void clear_screen(void);
void display(char *out);
