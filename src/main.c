#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#endif

#include "common.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#include "gif.h"
#pragma GCC diagnostic pop

struct GLOB glob;

#define STR_CMP(_str, _name, _alt, _code) \
    if(!strcmp(_str, _name) || !strcmp(_str, _alt)) {_code continue;}

typedef enum {
  INVALID,
  VALID,
  HELP
} parse_result_t;

void shift_args(int *argc, char ***argv) {
  *argc = *argc-2;
  *argv = *argv+2;
}

parse_result_t parse_args(int argc, char **argv) {
  // ignore program name
  argc--; argv++;

  // default vals
  glob.filename = NULL;
  glob.out_w = -1; glob.out_h = -1;
  glob.loop_arg = NOT_SET;

  while(argc > 0) {
    char *field = argv[0];
    char *val   = argv[1];
    shift_args(&argc, &argv);

    STR_CMP(field, "-f", "--file", {
      glob.filename = val;
    })
    STR_CMP(field, "-w", "--width", {
      int width = atoi(val);
      if(width == 0) return INVALID;
      glob.out_w = width;
    })
    STR_CMP(field, "-h", "--height", {
      int height = atoi(val);
      if(height == 0) return INVALID; 
      glob.out_h = height;
    })

    STR_CMP(field, "--loop", "-l", {
      glob.loop_arg = LOOP;
      argc++;
      argv--;
    })
    STR_CMP(field, "--no-loop", "-nl", {
      glob.loop_arg = NOT_LOOP;
      argc++;
      argv--;
    })

    STR_CMP(field, "--help", "-H", {
      return HELP;
    })

    // if we're still here, the user supplied a weird argument
    return INVALID;
  }

  if(glob.out_w == -1 || glob.filename == NULL)
    return INVALID;

  return VALID;
}

void help_output() {
  puts("Usage: ascii_converter [option] [value]");
  puts("Options:");
  puts("-f  or --file       : the input file path");
  puts("-w  or --width      : the output width");
  puts("-h  or --height     : the output height (optional)");
  puts("-l  or --loop       : set loop behaviour for gifs (optional)");
  puts("-nl or --no-loop    : do not set loop behaviour for gifs (optional)");
  puts("-H  or --help       : displays this message (optional)");
}

int main(int argc, char **argv) {
  switch(parse_args(argc, argv)) {
    case INVALID:
      FAIL
    case HELP:
      help_output();
      exit(0);
    case VALID:
      break;
  }

  if(strstr(glob.filename, ".gif")) { // handle gifs
    gif_t gif = gif_load_file(glob.filename);

    if(glob.loop_arg == NOT_SET) {
      gif_play(gif,
            #ifdef GIF_LOOP
              true
            #else
              false
            #endif
               );
    }
    else {
      gif_play(gif, glob.loop_arg == LOOP);
    }

    gif_free(gif);
  }
  else { // handle normal images
    // open image
    unsigned char *img = stbi_load(glob.filename, &glob.w, &glob.h, &glob.chs, 0);
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
