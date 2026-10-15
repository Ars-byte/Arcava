#include <cstdio>
#include <cstdlib>
#include <string>

#include <unistd.h>

namespace {
void usage(const char* prog) {
    std::fprintf(stderr,
        "Usage: %s [options]\n"
        "  -r <rate>      sample rate (default 44100)\n"
        "  -c <channels>  channels (default 2)\n"
        "  -d <device>    parec device (default @DEFAULT_MONITOR@)\n"
        "  -b <bars>      number of bars, 0 = auto (default 0)\n"
        "  -w <width>     bar width in cells (default 1)\n"
        "  -g <gap>       min gap between bars in cells (default 1)\n"
        "  -h             show this help\n"
        "If stdin is not a tty, raw s16le PCM is read from stdin.\n",
        prog);
}
}

int main(int argc, char** argv) {
    int rate = 44100;
    int channels = 2;
    int bars = 0;
    int barWidth = 1;
    int gap = 1;
    std::string device = "@DEFAULT_MONITOR@";

    int opt;
    while ((opt = getopt(argc, argv, "r:c:d:b:w:g:h")) != -1) {
        switch (opt) {
            case 'r': rate = std::atoi(optarg); break;
            case 'c': channels = std::atoi(optarg); break;
            case 'd': device = optarg; break;
            case 'b': bars = std::atoi(optarg); break;
            case 'w': barWidth = std::atoi(optarg); break;
            case 'g': gap = std::atoi(optarg); break;
            case 'h': usage(argv[0]); return 0;
            default: usage(argv[0]); return 1;
        }
    }

    if (rate <= 0) rate = 44100;
    if (channels <= 0) channels = 2;
    if (barWidth < 1) barWidth = 1;
    if (gap < 0) gap = 0;
    if (bars < 0) bars = 0;

    return 0;
}
