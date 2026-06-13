#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "buffer.h"
#include "colors.h"
#include "draw.h"

#define COMMAND_LINE_SIZE 256

#define INSERT_STATUS_LINE BWHITE "-- INSERT --" RESET
#define VISUAL_STATUS_LINE BWHITE "-- VISUAL --" RESET
#define VISUAL_LINE_STATUS_LINE BWHITE "-- VISUAL LINE --" RESET

typedef enum {
    NORMAL,
    INSERT,
    VISUAL,
    VISUAL_LINE,
    COMMAND,
    EXIT
} Mode;

typedef struct {
    char line[COMMAND_LINE_SIZE];
    size_t cursor_col;
    size_t length;
} CommandLine;

typedef struct {
    Buffer buffer;
    Position cursor;
    bool successful_motion;
    bool save_curosr_col;
    size_t prev_cursor_col;
    Position text;
    bool number;
    bool relative_number;
    char key_cache_string[KEY_CACHE_STRING_SIZE + 1];
    char cursor_position_string[CURSOR_POSITION_STRING_SIZE + 1];
    CommandLine command_line;
    bool saved;
    bool in_start;
    Mode mode;
    char* filename;
} Editor;

void editor_init(Editor* editor, char* filename);
void free_editor(Editor* editor);
