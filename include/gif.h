#pragma once

#include "gif_load/gif_load.h"

typedef struct {
  void *buff;
  size_t size;
} gif_t;

gif_t gif_load_file(const char *filename);
void  gif_free(gif_t load_ret);
void  gif_play(gif_t load_ret, bool loop);
void  gif_callback(void *, struct GIF_WHDR *frame);
