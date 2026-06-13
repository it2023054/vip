#include "movement.h"

#include <stdint.h>

#include "motion.h"
#include "vip.h"

static void fix_cursor_col(Editor* editor) {
    size_t line_length = get_line(editor->cursor.row)->length;
    if (editor->mode != INSERT && line_length > 0) {
        line_length--;
    }

    if (editor->prev_cursor_col >= line_length) {
        editor->cursor.col = line_length;
        return;
    }

    editor->cursor.col = editor->prev_cursor_col;
}

static void movement_left(Editor* editor, Motion motion, const size_t count) {
    if (editor->cursor.col == 0) {
        editor->save_curosr_col = false;
        return;
    }

    editor->cursor.col = get_motion_col_left(editor, motion, count, '\0');
}

static void movement_up(Editor* editor, Motion motion, const size_t count) {
    editor->save_curosr_col = false;

    size_t new_row = get_motion_row(editor, motion, count);
    if (new_row == editor->cursor.row) {
        return;
    }

    editor->cursor.row = new_row;

    fix_cursor_col(editor);
}

static void movement_down(Editor* editor, Motion motion, const size_t count) {
    editor->save_curosr_col = false;

    size_t new_row = get_motion_row(editor, motion, count);
    if (new_row == editor->cursor.row) {
        return;
    }

    editor->cursor.row = new_row;

    fix_cursor_col(editor);
}

static void movement_right(Editor* editor, Motion motion, const size_t count) {
    if (get_line(editor->cursor.row)->length == 0) {
        editor->save_curosr_col = false;
        return;
    }

    if (editor->mode == INSERT) {
        if (editor->cursor.col == get_line(editor->cursor.row)->length) {
            editor->save_curosr_col = false;
            return;
        }

        editor->cursor.col++;
        return;
    }

    size_t new_col = get_motion_col_right(editor, motion, count, '\0');
    if (new_col == editor->cursor.col) {
        editor->save_curosr_col = false;
        return;
    }

    editor->cursor.col = new_col;
}

static void movement_first_line(Editor* editor, Motion motion, const size_t count) {
    editor->save_curosr_col = false;

    editor->cursor.row = get_motion_row(editor, motion, count);

    fix_cursor_col(editor);
}

static void movement_last_line(Editor* editor, Motion motion, const size_t count) {
    editor->save_curosr_col = false;

    editor->cursor.row = get_motion_row(editor, motion, count);

    fix_cursor_col(editor);
}

static void movement_end_of_line(Editor* editor, Motion motion) {
    editor->save_curosr_col = false;
    editor->prev_cursor_col = SIZE_MAX;

    if (get_line(editor->cursor.row)->length == 0) {
        return;
    }

    editor->cursor.col = get_motion_col_right(editor, motion, 1, '\0');
}

bool is_movement_to_execute(Editor* editor, Motion motion, const size_t count) {
    switch (motion) {
        case MOT_LEFT:
            movement_left(editor, motion, count);
            break;
        case MOT_UP:
            movement_up(editor, motion, count);
            break;
        case MOT_DOWN:
            movement_down(editor, motion, count);
            break;
        case MOT_RIGHT:
            movement_right(editor, motion, count);
            break;
        case MOT_FIRST_LINE:
            movement_first_line(editor, motion, count);
            break;
        case MOT_LAST_LINE:
            movement_last_line(editor, motion, count);
            break;
        case MOT_END_OF_LINE:
            movement_end_of_line(editor, motion);
            break;
        default:
            return false;
    }

    return true;
}

void movement_line(Editor* editor, size_t row) {
    editor->save_curosr_col = false;

    row--;
    editor->cursor.row = row > get_line_count() - 1 ? get_line_count() - 1 : row;

    fix_cursor_col(editor);
}

void insert_arrow_left(Editor* editor) {
    movement_left(editor, MOT_LEFT, 1);
}

void insert_arrow_up(Editor* editor) {
    movement_up(editor, MOT_UP, 1);
}

void insert_arrow_down(Editor* editor) {
    movement_down(editor, MOT_DOWN, 1);
}

void insert_arrow_right(Editor* editor) {
    movement_right(editor, MOT_RIGHT, 1);
}
