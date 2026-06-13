#pragma once

#include <stddef.h>

typedef struct {
    size_t row;
    size_t col;
} Position;

void enable_raw_mode(void);
void disable_raw_mode(void);

void update_window_size(void);

Position get_max_screen(void);
Position get_max_buffer(void);
