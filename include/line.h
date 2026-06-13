#pragma once

#include <stddef.h>

typedef struct {
    char* chars;
    size_t length;
    size_t capacity;
} Line;

void line_init(Line* line);
void free_line(Line* line);
void line_ensure(Line* line, size_t needed);
void line_compact(Line* line);
