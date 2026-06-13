#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "editor.h"
#include "terminal.h"

typedef enum {
    MOT_NONE,

    MOT_UP,
    MOT_DOWN,
    MOT_LINE,
    MOT_FIRST_LINE,
    MOT_LAST_LINE,
    MOT_NEXT_PARAGRAPH,
    MOT_PREVIOUS_PARAGRAPH,

    MOT_LEFT,
    MOT_BACKWARD_WORD,
    MOT_BACKWARD_BIG_WORD,
    MOT_START_OF_LINE,
    MOT_FIRST_NON_BLANK_CHAR_OF_LINE,
    MOT_PREVIOUS_OCCURRENCE_OF_CHAR,
    MOT_AFTER_PREVIOUS_OCCURRENCE_OF_CHAR,

    MOT_RIGHT,
    MOT_WORD,
    MOT_BIG_WORD,
    MOT_END_WORD,
    MOT_END_BIG_WORD,
    MOT_END_OF_LINE,
    MOT_NEXT_OCCURRENCE_OF_CHAR,
    MOT_BEFORE_NEXT_OCCURRENCE_OF_CHAR,

    MOT_MATCHING_CHAR,
} Motion;

typedef enum {
    MOT_TYPE_NONE,
    MOT_TYPE_ROW,
    MOT_TYPE_COL_LEFT,
    MOT_TYPE_COL_RIGHT,
    MOT_TYPE_POSITION
} MotionType;

MotionType get_motion_type(Motion motion);

bool is_word_split(char c);
bool is_big_word_split(char c);

size_t get_motion_row(Editor* editor, Motion motion, const size_t count);
size_t get_motion_col(Editor* editor, Motion motion);
size_t get_motion_col_left(Editor* editor, Motion motion, const size_t count, const char c);
size_t get_motion_col_right(Editor* editor, Motion motion, const size_t count, const char c);
Position get_motion_position(Editor* editor, Motion motion, const size_t count);
