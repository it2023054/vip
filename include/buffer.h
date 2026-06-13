#pragma once

#include <stddef.h>

#include "line.h"
#include "terminal.h"

typedef struct {
    Line* lines;
    size_t line_count;
    size_t capacity;
} Buffer;

void buffer_init(Buffer* buffer);
void free_buffer(Buffer* buffer);

void insert_char(Buffer* buffer, Position* cursor, const char c);
void insert_string(Buffer* buffer, Position* cursor, const char* text);
void insert_enter(Buffer* buffer, Position* cursor);
void insert_backspace(Buffer* buffer, Position* cursor);

void replace_char(Buffer* buffer, Position* cursor, const size_t count, const char c);

void append_line(Buffer* buffer, Position* cursor);
void prepend_line(Buffer* buffer, Position* cursor);

void delete_row(Buffer* buffer, Position* cursor, size_t row);
void delete_col_left(Buffer* buffer, Position* cursor, size_t col);
void delete_col_right(Buffer* buffer, Position* cursor, size_t col);

void join_lines(Buffer* buffer, Position* cursor, size_t next_row_first_col);
