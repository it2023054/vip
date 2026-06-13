#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "editor.h"
#include "motion.h"

bool is_movement_to_execute(Editor* editor, Motion motion, const size_t count);

void movement_line(Editor* editor, size_t row);

void insert_arrow_left(Editor* editor);
void insert_arrow_up(Editor* editor);
void insert_arrow_down(Editor* editor);
void insert_arrow_right(Editor* editor);
