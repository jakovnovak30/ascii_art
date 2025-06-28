#include "gif.h"
#include "common.h"

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <threads.h>

gif_t gif_load_file(const char *filename) {
  FILE *gif_f = fopen(filename, "rb");
  if(gif_f == NULL) FAIL
  fseek(gif_f, 0, SEEK_END); // find end
  size_t size = ftell(gif_f); // get offset from start (file size)
  fseek(gif_f, 0, SEEK_SET); // return to start
  
  void *buff = malloc(size * sizeof(char));
  if(fread(buff, sizeof(char), size, gif_f) == 0) FAIL
  fclose(gif_f);

  return (gif_t){ buff, size };
}

void gif_free(gif_t load_ret) {
  free(load_ret.buff);
}

void gif_play(gif_t gif, bool loop) {
  unsigned char **fr_data = malloc(sizeof(unsigned char *));

  do {
    GIF_Load(gif.buff, gif.size, gif_callback, NULL, fr_data, 0);
  } while(loop);

  //free(fr_data);
}

void gif_callback(void *prev, struct GIF_WHDR *frame) {
  /*
  LOG("frxd=%ld, fryd=%ld, frxo=%ld, fryo=%ld",
      frame->frxd, frame->fryd, frame->frxo, frame->fryo);
  LOG("intr=%ld", frame->intr);
  */

  if(frame->ifrm == 0) {
    glob.w = frame->xdim;
    glob.h = frame->ydim;
    CALCULATE_GLOB
    glob.chs = 3;

    *((unsigned char **)prev) = malloc(glob.w * glob.h * glob.chs);
  }
  unsigned char *fr_data = *((unsigned char **)prev);

  char *out = malloc((glob.out_w+1)*glob.out_h*sizeof(char));
  for(int hh=0;hh < glob.h && hh < frame->fryd;hh++) {
    for(int ww=0;ww < glob.w && ww < frame->frxd;ww++) {
      int index = (hh + frame->fryo) * glob.w * glob.chs + (ww + frame->frxo) * glob.chs;

      if(frame->tran != frame->bptr[hh*frame->frxd+ww]) {
        fr_data[index + 0] = frame->cpal[frame->bptr[hh*frame->frxd+ww]].R;
        fr_data[index + 1] = frame->cpal[frame->bptr[hh*frame->frxd+ww]].G;
        fr_data[index + 2] = frame->cpal[frame->bptr[hh*frame->frxd+ww]].B;
      }
      else {
        fr_data[index + 0] = '\0';
        fr_data[index + 1] = '\0';
        fr_data[index + 2] = '\0';
      }
    }
  }

  convert_frame(fr_data, out);
  display(out);
  clear_screen();
  free(out);

  // gif time units (1 unit = 10 msec)
  thrd_sleep(&(struct timespec){ .tv_nsec = frame->time * 1e7 }, NULL);
}
