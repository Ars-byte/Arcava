#pragma once

#include <cstddef>

void termInit();
void termRestore();
void termWrite(const char* data, size_t n);
void termSize(int& width, int& height);
