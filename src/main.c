#include <stdlib.h>
#include <string.h>
#include <threads.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#endif

#include "common.h"
#include "gif.h"

struct GLOB glob;

void parse_args(int argc, char **argv) {
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
}

int main(int argc, char **argv) {
  parse_args(argc, argv);

  if(strstr(argv[1], ".gif")) { // handle gifs
    gif_t gif = gif_load_file(argv[1]);

    #ifdef GIF_LOOP
    gif_play(gif, true);
    #else
    gif_play(gif, false);
    #endif

    gif_free(gif);
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
