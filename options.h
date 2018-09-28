#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdlib.h>
#include <unistd.h>

struct Options {
  Options() = default;
  Options(int argc, char* argv[]) {
    int c;
    while ((c = getopt(argc, argv, "acn:")) != -1) {
      switch (c) {
        case 'a': auto_play = true; break;
        case 'c': minimize_color_diff = true; break;
        case 'n': num_beams = atoi(optarg); break;
      }
    }
    if (optind < argc) seed = atoi(argv[optind]);
    if (optind + 1 < argc) beam_size = 1 << atoi(argv[optind + 1]);
  }

  int seed = 1;
  int beam_size = 1 << 15;
  int num_beams = 1;
  bool minimize_color_diff = false;
  bool auto_play = false;
};

extern Options options;

#endif
