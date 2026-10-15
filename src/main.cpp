#include "analyzer.hpp"
#include "audio.hpp"
#include "fft.hpp"
#include "render.hpp"
#include "term.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

#include <unistd.h>

namespace {
const float kPi = 3.14159265358979323846f;
const int kFps = 60;
const char* kVersion = "1.0.0";

void usage(const char* prog) {
    std::fprintf(stderr,
        "Usage: %s [options]\n"
        "  -v, --v        show version\n"
        "  --version      show version\n"
        "  -h             show this help\n"
        "  -r <rate>      sample rate (default 44100)\n"
        "  -c <channels>  channels (default 2)\n"
        "  -d <device>    parec device (default @DEFAULT_MONITOR@)\n"
        "  -b <bars>      number of bars, 0 = auto (default 0)\n"
        "  -w <width>     bar width in cells (default 1)\n"
        "  -g <gap>       min gap between bars in cells (default 1)\n"
        "If stdin is not a tty, raw s16le PCM is read from stdin.\n",
        prog);
}
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        std::string a = argv[i];
        if (a == "--v" || a == "-v" || a == "--version") {
            std::printf("%s %s\n", argv[0], kVersion);
            return 0;
        }
        if (a == "--help" || a == "-h" || a == "-H") {
            usage(argv[0]);
            return 0;
        }
    }
    int rate = 44100;
    int channels = 2;
    int bars = 0;
    int barWidth = 1;
    int gap = 1;
    std::string device = "@DEFAULT_MONITOR@";
    std::string color = "\033[38;5;231m";

    int opt;

    while ((opt = getopt(argc, argv, "r:c:d:b:w:g:h:C:")) != -1) {
        switch (opt) {
            case 'r': rate = std::atoi(optarg); break;
            case 'c': channels = std::atoi(optarg); break;
            case 'd': device = optarg; break;
            case 'b': bars = std::atoi(optarg); break;
            case 'w': barWidth = std::atoi(optarg); break;
            case 'g': gap = std::atoi(optarg); break;
            case 'C': color = optarg; break;
            case 'h': usage(argv[0]); return 0;
            case '?': usage(argv[0]); return 1;
        }
    }
    if (rate <= 0) rate = 44100;
    if (channels <= 0) channels = 2;
    if (barWidth < 1) barWidth = 1;
    if (gap < 0) gap = 0;
    if (bars < 0) bars = 0;

    AudioSource audio(rate, channels, device);
    if (!audio.good()) {
        std::perror("parec");
        return 1;
    }

    int N = (int)audio.windowFrames();
    std::vector<std::complex<float>> spec(N);
    std::vector<float> hann(N);
    for (int i = 0; i < N; i++)
        hann[i] = 0.5f * (1.0f - std::cos(2.0f * kPi * i / (N - 1)));

    termInit();

    if (!audio.init()) {
        termRestore();
        return 0;
    }

    Analyzer analyzer(rate);
    std::vector<float> level;
    std::string frame;
    int ch = audio.channels();
    const float frameBudget = 1.0f / kFps;
    auto last = std::chrono::steady_clock::now();

    while (true) {
        auto frameStart = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(frameStart - last).count();
        last = frameStart;
        if (dt <= 0.0f) dt = frameBudget;
        if (dt > 0.25f) dt = 0.25f;

        const int16_t* win = audio.window();
        for (int i = 0; i < N; i++) {
            float acc = 0.0f;
            for (int c = 0; c < ch; c++) acc += (float)win[i * ch + c];
            acc /= ch;
            spec[i] = std::complex<float>(acc * hann[i] / 32768.0f, 0.0f);
        }
        fft(spec);

        int width, rows;
        termSize(width, rows);
        if (rows < 1) rows = 1;
        int cell = barWidth + gap;
        int maxBars = std::max(1, width / barWidth);
        int nb = bars > 0 ? bars : (width + gap) / cell;
        nb = std::max(1, std::min(nb, maxBars));

        analyzer.configure(nb);
        analyzer.process(spec.data(), dt, level);
        renderFrame(frame, level, width, rows, barWidth, color);
        termWrite(frame.data(), frame.size());

        if (!audio.advance()) break;

        float elapsed = std::chrono::duration<float>(
            std::chrono::steady_clock::now() - frameStart).count();
        if (elapsed < frameBudget)
            std::this_thread::sleep_for(std::chrono::duration<float>(frameBudget - elapsed));
    }

    termRestore();
    return 0;
}
