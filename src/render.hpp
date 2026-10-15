#pragma once

#include <string>
#include <vector>

void renderFrame(std::string& buf, const std::vector<float>& level,
                 int width, int rows, int barWidth);
