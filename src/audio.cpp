#include "audio.hpp"

#include <cstring>

#include <unistd.h>

namespace {
const int kFftSize = 4096;
const int kFps = 60;
}

AudioSource::AudioSource(int rate, int channels, const std::string& device)
    : rate_(rate), channels_(channels), N_(kFftSize) {
    hop_ = rate_ / kFps;
    if (hop_ < 1) hop_ = 1;
    win_.assign((size_t)N_ * channels_, 0);
    chunk_.assign((size_t)hop_ * channels_, 0);

    if (isatty(STDIN_FILENO)) {
        std::string cmd = "parec --format=s16le --rate=" + std::to_string(rate_) +
                          " --channels=" + std::to_string(channels_) +
                          " --latency-msec=20 --device=" + device + " 2>/dev/null";
        pipe_ = popen(cmd.c_str(), "r");
        in_ = pipe_;
    } else {
        in_ = stdin;
    }
}

AudioSource::~AudioSource() {
    if (pipe_) pclose(pipe_);
}

size_t AudioSource::readFull(int16_t* dst, size_t count) {
    size_t total = 0;
    while (total < count) {
        size_t n = fread(dst + total, sizeof(int16_t), count - total, in_);
        if (n == 0) break;
        total += n;
    }
    return total;
}

bool AudioSource::init() {
    if (!in_) return false;
    return readFull(win_.data(), win_.size()) == win_.size();
}

bool AudioSource::advance() {
    if (readFull(chunk_.data(), chunk_.size()) < chunk_.size()) return false;
    std::memmove(win_.data(), win_.data() + (size_t)hop_ * channels_,
                 (size_t)(N_ - hop_) * channels_ * sizeof(int16_t));
    std::memcpy(win_.data() + (size_t)(N_ - hop_) * channels_, chunk_.data(),
                (size_t)hop_ * channels_ * sizeof(int16_t));
    return true;
}
