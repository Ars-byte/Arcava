#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

class AudioSource {
public:
    AudioSource(int rate, int channels, const std::string& device);
    ~AudioSource();

    bool good() const { return in_ != nullptr; }
    bool init();
    bool advance();

    const int16_t* window() const { return win_.data(); }
    size_t windowFrames() const { return (size_t)N_; }
    int channels() const { return channels_; }

private:
    size_t readFull(int16_t* dst, size_t count);

    int rate_;
    int channels_;
    int N_;
    int hop_;
    FILE* in_ = nullptr;
    FILE* pipe_ = nullptr;
    std::vector<int16_t> win_;
    std::vector<int16_t> chunk_;
};
