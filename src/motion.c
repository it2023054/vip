#include "motion.h"

#include "editor.h"
#include "error.h"
#include "keys.h"
#include "vip.h"

MotionType get_motion_type(Motion motion) {
    switch (motion) {
        case MOT_UP:
        case MOT_DOWN:
        case MOT_LINE:
        case MOT_FIRST_LINE:
        case MOT_LAST_LINE:
        case MOT_NEXT_PARAGRAPH:
        case MOT_PREVIOUS_PARAGRAPH:
            return MOT_TYPE_ROW;
        case MOT_LEFT:
        case MOT_BACKWARD_WORD:
        case MOT_BACKWARD_BIG_WORD:
        case MOT_START_OF_LINE:
        case MOT_FIRST_NON_BLANK_CHAR_OF_LINE:
        case MOT_PREVIOUS_OCCURRENCE_OF_CHAR:
        case MOT_AFTER_PREVIOUS_OCCURRENCE_OF_CHAR:
            return MOT_TYPE_COL_LEFT;
        case MOT_RIGHT:
        case MOT_WORD:
        case MOT_BIG_WORD:
        case MOT_END_WORD:
        case MOT_END_BIG_WORD:
        case MOT_END_OF_LINE:
        case MOT_NEXT_OCCURRENCE_OF_CHAR:
        case MOT_BEFORE_NEXT_OCCURRENCE_OF_CHAR:
            return MOT_TYPE_COL_RIGHT;
        case MOT_MATCHING_CHAR:
            return MOT_TYPE_POSITION;
        default:
            ERROR("Wrong motion");
            return MOT_TYPE_NONE;
    }
}

bool is_word_split(char c) {
    return !((IS_DIGIT(c)) || (IS_UPPER_CASE_CHAR(c)) || (IS_LOWER_CASE_CHAR(c)));
}

bool is_big_word_split(char c) {
    return c == SPACE;
}

static size_t motion_up(Editor* editor, const size_t count) {
    if (editor->cursor.row == 0) {
        editor->successful_motion = false;
        return 0;
    }

    if (count >= editor->cursor.row) {
        return 0;
    }

    return editor->cursor.row - count;
}

static size_t motion_down(Editor* editor, const size_t count) {
    if (editor->cursor.row == get_line_count() - 1) {
        editor->successful_motion = false;
        return get_line_count() - 1;
    }

    if (editor->cursor.row + count >= get_line_count() - 1) {
        return get_line_count() - 1;
    }

    return editor->cursor.row + count;
}

static size_t motion_line(size_t row) {
    row--;

    if (row > get_line_count() - 1) {
        return get_line_count() - 1;
    }

    return row;
}

static size_t motion_first_line(const size_t count) {
    if (count > 1) {
        return motion_line(count);
    }

    return 0;
}

static size_t motion_last_line(const size_t count) {
    if (count > 1) {
        return motion_line(count);
    }

    return get_line_count() - 1;
}

static size_t motion_next_paragraph(Editor* editor, const size_t count) {
    editor->cursor.col = 0;

    size_t line_count = editor->buffer.line_count;

    if (editor->cursor.row == line_count - 1) {
        editor->successful_motion = false;
        return line_count - 1;
    }

    size_t found = 0;

    for (size_t i = editor->cursor.row + 1; i < line_count - 1; i++) {
        if (get_line(i)->length == 0) {
            found++;

            if (found == count) {
                return i;
            }
        }
    }

    return line_count - 1;
}

static size_t motion_previous_paragraph(Editor* editor, const size_t count) {
    editor->cursor.col = 0;

    if (editor->cursor.row == 0) {
        editor->successful_motion = false;
        return 0;
    }

    size_t found = 0;

    for (size_t i = editor->cursor.row - 1; i > 0; i--) {
        if (get_line(i)->length == 0) {
            found++;

            if (found == count) {
                return i;
            }
        }
    }

    return 0;
}

static size_t motion_left(Editor* editor, const size_t count) {
    if (editor->cursor.col == 0) {
        editor->successful_motion = false;
        return 0;
    }

    if (count >= editor->cursor.col) {
        return 0;
    }

    return editor->cursor.col - count;
}

static size_t motion_backward_word(Editor* editor, const size_t count) {
    if (editor->cursor.col == 0) {
        editor->successful_motion = false;
        return 0;
    }

    Line* line = get_line(editor->cursor.row);
    size_t found = 0;

    size_t prev_col = editor->cursor.col;
    while (found < count) {
        prev_col--;
        if (prev_col == 0) {
            return 0;
        }

        if (is_word_split(line->chars[prev_col]) && line->chars[prev_col] != SPACE) {
            found++;
            continue;
        }

        bool foundWord = false;
        for (size_t i = prev_col - 1; i > 0; i--) {
            if (is_word_split(line->chars[i])) {
                if (line->chars[i] == SPACE && line->chars[i + 1] == SPACE) {
                    continue;
                }

                foundWord = true;
                prev_col = i + 1;
                found++;
                break;
            }
        }

        if (!foundWord) {
            return 0;
        }
    }

    return prev_col;
}

static size_t motion_backward_big_word(Editor* editor, const size_t count) {
    if (editor->cursor.col == 0) {
        editor->successful_motion = false;
        return 0;
    }

    Line* line = get_line(editor->cursor.row);
    size_t found = 0;

    size_t prev_col = editor->cursor.col;
    while (found < count) {
        prev_col--;
        if (prev_col == 0) {
            return 0;
        }

        bool foundWord = false;
        for (size_t i = prev_col - 1; i > 0; i--) {
            if (is_big_word_split(line->chars[i])) {
                if (line->chars[i] == SPACE && line->chars[i + 1] == SPACE) {
                    continue;
                }

                foundWord = true;
                prev_col = i + 1;
                found++;
                break;
            }
        }

        if (!foundWord) {
            return 0;
        }
    }

    return prev_col;
}

static size_t motion_start_of_line(Editor* editor) {
    if (editor->cursor.col == 0) {
        editor->successful_motion = false;
        return 0;
    }

    return 0;
}

static size_t motion_first_non_blank_char_of_line(Editor* editor) {
    Line* line = get_line(editor->cursor.row);

    for (size_t i = 0; i < line->length - 1; i++) {
        if (line->chars[i] != SPACE) {
            return i;
        }
    }

    return line->length - 1;
}

static size_t motion_previous_occurrence_of_char(Editor* editor, const size_t count, const char c) {
    if (editor->cursor.col == 0) {
        editor->successful_motion = false;
        return 0;
    }

    Line* line = get_line(editor->cursor.row);
    size_t found = 0;

    size_t prev_col = editor->cursor.col;
    while (found < count) {
        prev_col--;

        if (line->chars[prev_col] == c) {
            found++;
        }

        if (prev_col == 0) {
            if (found == count) {
                break;
            }

            editor->successful_motion = false;
            return 0;
        }
    }

    return prev_col;
}

static size_t motion_after_previous_occurrence_of_char(Editor* editor, const size_t count, const char c) {
    if (editor->cursor.col == 0) {
        editor->successful_motion = false;
        return 0;
    }

    Line* line = get_line(editor->cursor.row);
    size_t found = 0;

    size_t prev_col = editor->cursor.col;
    while (found < count) {
        prev_col--;

        if (line->chars[prev_col] == c) {
            found++;
        }

        if (prev_col == 0) {
            if (found == count) {
                break;
            }

            editor->successful_motion = false;
            return 0;
        }
    }

    return prev_col + 1;
}

static size_t motion_right(Editor* editor, const size_t count) {
    if (editor->cursor.col + count >= get_line(editor->cursor.row)->length - 1) {
        return get_line(editor->cursor.row)->length - 1;
    }

    return editor->cursor.col + count;
}

static size_t motion_word(Editor* editor, const size_t count) {
    Line* line = get_line(editor->cursor.row);

    if (editor->cursor.col == line->length - 1) {
        editor->successful_motion = false;
        return editor->cursor.col;
    }

    size_t found = 0;

    size_t next_col = editor->cursor.col;
    while (found < count) {
        next_col++;
        if (next_col == line->length) {
            return line->length - 1;
        }

        if (is_word_split(line->chars[next_col]) && line->chars[next_col] != SPACE) {
            found++;
            continue;
        }

        if (line->chars[next_col - 1] == SPACE) {
            next_col--;
        } else if (is_word_split(line->chars[next_col - 1])) {
            bool foundNotSpace = false;
            for (size_t i = next_col; i < line->length - 1; i++) {
                if (line->chars[i] != SPACE) {
                    foundNotSpace = true;
                    next_col = i;
                    found++;
                    break;
                }
            }

            if (!foundNotSpace) {
                return line->length - 1;
            }

            continue;
        }

        bool foundWord = false;
        for (size_t i = next_col; i < line->length - 1; i++) {
            if (is_word_split(line->chars[i])) {
                if (line->chars[i] == SPACE && line->chars[i + 1] == SPACE) {
                    continue;
                }

                foundWord = true;
                next_col = line->chars[i] == SPACE ? i + 1 : i;
                found++;
                break;
            }
        }

        if (!foundWord) {
            return line->length - 1;
        }
    }

    return next_col;
}

static size_t motion_big_word(Editor* editor, const size_t count) {
    Line* line = get_line(editor->cursor.row);

    if (editor->cursor.col == line->length - 1) {
        editor->successful_motion = false;
        return editor->cursor.col;
    }

    size_t found = 0;

    size_t next_col = editor->cursor.col;
    while (found < count) {
        bool foundWord = false;
        for (size_t i = next_col; i < line->length - 1; i++) {
            if (is_big_word_split(line->chars[i])) {
                if (line->chars[i] == SPACE && line->chars[i + 1] == SPACE) {
                    continue;
                }

                foundWord = true;
                next_col = i;
                found++;
                break;
            }
        }

        if (!foundWord) {
            return line->length - 1;
        }

        next_col++;
        if (next_col == line->length) {
            return next_col - 1;
        }
    }

    return next_col;
}

static size_t motion_end_word(Editor* editor, const size_t count) {
    Line* line = get_line(editor->cursor.row);

    if (editor->cursor.col == line->length - 1) {
        editor->successful_motion = false;
        return editor->cursor.col;
    }

    size_t found = 0;

    size_t next_col = editor->cursor.col;
    while (found < count) {
        next_col++;
        if (next_col == line->length) {
            return line->length - 1;
        }

        if (is_word_split(line->chars[next_col]) && line->chars[next_col] != SPACE) {
            found++;
            continue;
        }

        bool foundWord = false;
        for (size_t i = next_col + 1; i < line->length - 1; i++) {
            if (is_word_split(line->chars[i])) {
                if (line->chars[i] == SPACE && line->chars[i - 1] == SPACE) {
                    continue;
                }

                foundWord = true;
                if (line->chars[i] == SPACE) {
                    next_col = i - 1;
                } else {
                    next_col = line->chars[i - 1] == SPACE ? i : i - 1;
                }
                found++;
                break;
            }
        }

        if (!foundWord) {
            return line->length - 1;
        }
    }

    return next_col;
}

static size_t motion_end_big_word(Editor* editor, const size_t count) {
    Line* line = get_line(editor->cursor.row);

    if (editor->cursor.col == line->length - 1) {
        editor->successful_motion = false;
        return editor->cursor.col;
    }

    size_t found = 0;

    size_t next_col = editor->cursor.col;
    while (found < count) {
        next_col++;
        if (next_col == line->length) {
            return line->length - 1;
        }

        bool foundWord = false;
        for (size_t i = next_col + 1; i < line->length - 1; i++) {
            if (is_big_word_split(line->chars[i])) {
                if (line->chars[i] == SPACE && line->chars[i - 1] == SPACE) {
                    continue;
                }

                foundWord = true;
                next_col = i - 1;
                found++;
                break;
            }
        }

        if (!foundWord) {
            return line->length - 1;
        }
    }

    return next_col;
}

static size_t motion_end_of_line(Editor* editor) {
    return get_line(editor->cursor.row)->length - 1;
}

static size_t motion_next_occurrence_of_char(Editor* editor, const size_t count, const char c) {
    Line* line = get_line(editor->cursor.row);

    if (editor->cursor.col == line->length - 1) {
        editor->successful_motion = false;
        return editor->cursor.col;
    }

    size_t found = 0;

    size_t next_col = editor->cursor.col;
    while (found < count) {
        next_col++;
        if (next_col == line->length) {
            editor->successful_motion = false;
            return editor->cursor.col;
        }

        if (line->chars[next_col] == c) {
            found++;
        }
    }

    return next_col;
}

static size_t motion_before_next_occurrence_of_char(Editor* editor, const size_t count, const char c) {
    Line* line = get_line(editor->cursor.row);

    if (editor->cursor.col == line->length - 1) {
        editor->successful_motion = false;
        return editor->cursor.col;
    }

    size_t found = 0;

    size_t next_col = editor->cursor.col;
    while (found < count) {
        next_col++;
        if (next_col == line->length) {
            editor->successful_motion = false;
            return editor->cursor.col;
        }

        if (line->chars[next_col] == c) {
            found++;
        }
    }

    return next_col - 1;
}

static Position motion_matching_char(Editor* editor) {
    Line* line = get_line(editor->cursor.row);

    char c = line->chars[editor->cursor.col];
    if (IS_LEFT_PAIR(c)) {
        size_t opened = 1;
        size_t row = editor->cursor.row;
        size_t col = editor->cursor.col;

        char c_pair = GET_LEFT_PAIR(c);
        while (opened > 0) {
            col++;
            if (col == line->length) {
                if (row == editor->buffer.line_count - 1) {
                    editor->successful_motion = false;
                    return editor->cursor;
                }

                row++;
                line = get_line(row);
                col = 0;
            }

            if (line->chars[col] == c) {
                opened++;
            } else if (line->chars[col] == c_pair) {
                opened--;
            }
        }

        return (Position){row, col};
    } else if (IS_RIGHT_PAIR(c)) {
        size_t closed = 1;
        size_t row = editor->cursor.row;
        size_t col = editor->cursor.col;

        char c_pair = GET_RIGHT_PAIR(c);
        while (closed > 0) {
            if (col > 0) {
                col--;
            } else {
                if (row == 0) {
                    editor->successful_motion = false;
                    return editor->cursor;
                }

                row--;
                line = get_line(row);
                col = line->length == 0 ? 0 : line->length - 1;
            }

            if (line->chars[col] == c) {
                closed++;
            } else if (line->chars[col] == c_pair) {
                closed--;
            }
        }

        return (Position){row, col};
    }

    editor->successful_motion = false;
    return editor->cursor;
}

size_t get_motion_row(Editor* editor, Motion motion, const size_t count) {
    switch (motion) {
        case MOT_UP:
            return motion_up(editor, count);
        case MOT_DOWN:
            return motion_down(editor, count);
        case MOT_LINE:
            return motion_line(editor->cursor.row + count);
        case MOT_FIRST_LINE:
            return motion_first_line(count);
        case MOT_LAST_LINE:
            return motion_last_line(count);
        case MOT_NEXT_PARAGRAPH:
            return motion_next_paragraph(editor, count);
        case MOT_PREVIOUS_PARAGRAPH:
            return motion_previous_paragraph(editor, count);
        default:
            ERROR("Wrong motion");
            return 0;
    }
}

size_t get_motion_col(Editor* editor, Motion motion) {
    if (editor->buffer.lines[editor->cursor.row].length == 0) {
        editor->successful_motion = false;
        return 0;
    }

    switch (motion) {
        case MOT_START_OF_LINE:
            return motion_start_of_line(editor);
        case MOT_FIRST_NON_BLANK_CHAR_OF_LINE:
            return motion_first_non_blank_char_of_line(editor);
        case MOT_END_OF_LINE:
            return motion_end_of_line(editor);
        default:
            ERROR("Wrong motion");
            return 0;
    }
}

size_t get_motion_col_left(Editor* editor, Motion motion, const size_t count, const char c) {
    if (editor->buffer.lines[editor->cursor.row].length == 0) {
        editor->successful_motion = false;
        return 0;
    }

    switch (motion) {
        case MOT_LEFT:
            return motion_left(editor, count);
        case MOT_BACKWARD_WORD:
            return motion_backward_word(editor, count);
        case MOT_BACKWARD_BIG_WORD:
            return motion_backward_big_word(editor, count);
        case MOT_START_OF_LINE:
            return motion_start_of_line(editor);
        case MOT_FIRST_NON_BLANK_CHAR_OF_LINE:
            return motion_first_non_blank_char_of_line(editor);
        case MOT_PREVIOUS_OCCURRENCE_OF_CHAR:
            return motion_previous_occurrence_of_char(editor, count, c);
        case MOT_AFTER_PREVIOUS_OCCURRENCE_OF_CHAR:
            return motion_after_previous_occurrence_of_char(editor, count, c);
        default:
            ERROR("Wrong motion");
            return 0;
    }
}

size_t get_motion_col_right(Editor* editor, Motion motion, const size_t count, const char c) {
    if (editor->buffer.lines[editor->cursor.row].length == 0) {
        editor->successful_motion = false;
        return 0;
    }

    switch (motion) {
        case MOT_RIGHT:
            return motion_right(editor, count);
        case MOT_WORD:
            return motion_word(editor, count);
        case MOT_BIG_WORD:
            return motion_big_word(editor, count);
        case MOT_END_WORD:
            return motion_end_word(editor, count);
        case MOT_END_BIG_WORD:
            return motion_end_big_word(editor, count);
        case MOT_END_OF_LINE:
            return motion_end_of_line(editor);
        case MOT_NEXT_OCCURRENCE_OF_CHAR:
            return motion_next_occurrence_of_char(editor, count, c);
        case MOT_BEFORE_NEXT_OCCURRENCE_OF_CHAR:
            return motion_before_next_occurrence_of_char(editor, count, c);
        default:
            ERROR("Wrong motion");
            return 0;
    }
}

Position get_motion_position(Editor* editor, Motion motion, const size_t count) {
    (void)count;

    switch (motion) {
        case MOT_MATCHING_CHAR:
            return motion_matching_char(editor);
        default:
            ERROR("Wrong motion");
            return (Position){0, 0};
    }
}
