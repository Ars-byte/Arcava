#include "analyzer.hpp"

#include <algorithm>
#include <cmath>

namespace {
const int kFftSize = 4096;
const double kLowFreq = 20.0;
const double kHighFreq = 20000.0;
}

Analyzer::Analyzer(int rate) : rate_(rate) {}

void Analyzer::configure(int bars) {
    if (bars == nb_) return;
    nb_ = bars;
    bandLo_.resize(nb_);
    bandHi_.resize(nb_);
    raw_.assign(nb_, 0.0f);

    double ratio = kHighFreq / kLowFreq;
    for (int b = 0; b < nb_; b++) {
        double f0 = kLowFreq * std::pow(ratio, (double)b / nb_);
        double f1 = kLowFreq * std::pow(ratio, (double)(b + 1) / nb_);
        int k0 = std::max(1, (int)(f0 * kFftSize / rate_));
        int k1 = std::max(k0 + 1, (int)(f1 * kFftSize / rate_));
        bandLo_[b] = k0;
        bandHi_[b] = std::min(k1, kFftSize / 2);
    }
}

void Analyzer::process(const std::complex<float>* spec, float dt, std::vector<float>& level) {
    if ((int)level.size() != nb_) level.assign(nb_, 0.0f);

    float frameMax = 1e-6f;
    for (int b = 0; b < nb_; b++) {
        float mag = 0.0f;
        for (int k = bandLo_[b]; k < bandHi_[b]; k++) mag += std::abs(spec[k]);
        mag /= (bandHi_[b] - bandLo_[b]);
        raw_[b] = mag;
        frameMax = std::max(frameMax, mag);
    }

    peak_ = std::max(peak_ * std::pow(0.5f, dt / 1.5f), frameMax);

    for (int b = 0; b < nb_; b++) {
        float r = raw_[b] / peak_;
        if (r > 1.0f) r = 1.0f;
        level[b] = std::min(1.0f, std::max(0.0f, r));
    }
}
