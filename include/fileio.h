#pragma once

#include "buffer.h"

void save_file(Buffer* buffer, const char* filename, size_t* lines_saved, size_t* bytes_saved);
void load_file(Buffer* buffer, const char* filename);
