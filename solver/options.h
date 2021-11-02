#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdlib.h>
#include <unistd.h>

struct Options {
  Options() = default;
  Options(int argc, char* argv[]) {
    int c;
    while ((c = getopt(argc, argv, "Aacd:n:q")) != -1) {
      switch (c) {
        case 'A': max_auto_play = true; break;
        case 'a': auto_play = true; break;
        case 'c': minimize_color_diff = true; break;
        case 'd': deal = atoi(optarg); break;
        case 'n': num_beams = atoi(optarg); break;
        case 'q': quiet = true; break;
      }
    }
    if (optind < argc) seed = atoi(argv[optind]);
    if (optind + 1 < argc) beam_size = 1 << atoi(argv[optind + 1]);
  }

  int seed = 1;
  int beam_size = 1 << 15;
  int num_beams = 1;
  int deal = 0;
  bool minimize_color_diff = false;
  bool max_auto_play = false;
  bool auto_play = false;
  bool quiet = false;
};

extern Options options;

#endif
