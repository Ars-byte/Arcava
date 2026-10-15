#include "term.hpp"

#include <csignal>
#include <cstdlib>
#include <cstring>

#include <sys/ioctl.h>
#include <unistd.h>

namespace {
volatile sig_atomic_t g_resized = 1;
int g_width = 80;
int g_height = 24;

void updateSize() {
    if (!g_resized) return;
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        if (w.ws_col > 0) g_width = w.ws_col;
        if (w.ws_row > 0) g_height = w.ws_row;
    }
    g_resized = 0;
}

void onResize(int) { g_resized = 1; }

void onExit(int) {
    termRestore();
    _exit(0);
}
}

void termInit() {
    signal(SIGINT, onExit);
    signal(SIGTERM, onExit);
    signal(SIGWINCH, onResize);
    const char* seq = "\033[?1049h\033[?25l\033[2J";
    termWrite(seq, std::strlen(seq));
}

void termRestore() {
    const char* seq = "\033[0m\033[?25h\033[?1049l";
    ssize_t n = ::write(STDOUT_FILENO, seq, std::strlen(seq));
    (void)n;
}

void termWrite(const char* data, size_t n) {
    for (const char* p = data, *end = data + n; p < end;) {
        ssize_t w = ::write(STDOUT_FILENO, p, (size_t)(end - p));
        if (w <= 0) break;
        p += w;
    }
}

void termSize(int& width, int& height) {
    updateSize();
    width = g_width;
    height = g_height;
}
