#include "render.hpp"

#include <algorithm>

namespace {
const char* kBlocks[] = {" ", "\u2581", "\u2582", "\u2583", "\u2584", "\u2585", "\u2586", "\u2587", "\u2588"};
const char* kWhite = "\033[97m";
}

void renderFrame(std::string& buf, const std::vector<float>& level,
                 int width, int rows, int barWidth) {
    int nb = (int)level.size();
    int totalBar = nb * barWidth;
    int gapBudget = std::max(0, width - totalBar);
    int nGaps = nb - 1;
    int baseGap = nGaps > 0 ? gapBudget / nGaps : 0;
    int extraGap = nGaps > 0 ? gapBudget % nGaps : 0;

    size_t need = (size_t)rows * ((size_t)width * 4 + 32) + 16;
    if (buf.capacity() < need) buf.reserve(need);
    buf.assign("\033[H");

    int cells = rows * 8;
    for (int r = 0; r < rows; r++) {
        buf += kWhite;
        int fromBottom = rows - 1 - r;
        int gi = 0;
        for (int b = 0; b < nb; b++) {
            int L = (int)(level[b] * cells);
            int full = L >> 3;
            int rem = L & 7;
            int val = (fromBottom < full) ? 8 : (fromBottom == full ? rem : 0);
            for (int w = 0; w < barWidth; w++) buf += kBlocks[val];
            if (b < nb - 1) {
                buf.append((size_t)(baseGap + (gi < extraGap)), ' ');
                gi++;
            }
        }
        buf += "\033[0m\033[K";
        if (r < rows - 1) buf += '\n';
    }
}
