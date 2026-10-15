#pragma once

#include <complex>
#include <vector>

class Analyzer {
public:
    explicit Analyzer(int rate);

    void configure(int bars);
    void process(const std::complex<float>* spec, float dt, std::vector<float>& level);

private:
    int rate_;
    int nb_ = 0;
    std::vector<int> bandLo_;
    std::vector<int> bandHi_;
    std::vector<float> raw_;
    std::vector<float> prev_;
    std::vector<float> cpeak_;
    std::vector<float> cfall_;
    std::vector<float> cmem_;
    float peak_ = 1.0f;
    float framerate_ = 60.0f;
};
