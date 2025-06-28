#include "gif.h"
#include "common.h"

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
  do {
    GIF_Load(gif.buff, gif.size, gif_callback, NULL, NULL, 0);
  } while(loop);
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
