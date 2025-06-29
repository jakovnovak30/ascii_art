#include "common.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

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
  // ANSI escape codes: https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
  printf("\x1b[%dA", glob.out_h);
  printf("\x1b[%dD", glob.out_w);
}

void display(char *out) {
  for(int hh=0;hh < glob.out_h;hh++) puts(&out[hh*(glob.out_w+1)]);
}

