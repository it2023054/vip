#include "normal.h"

#include <stdbool.h>
#include <stdint.h>

#include "error.h"
#include "keys.h"
#include "motion.h"
#include "movement.h"

static void fix_count(Parser* parser) {
    if (parser->cmd.count == 0) {
        parser->cmd.count = 1;
    }

    if (parser->cmd.count_after_operator == 0) {
        parser->cmd.count_after_operator = 1;
    }

    parser->cmd.count = parser->cmd.count * parser->cmd.count_after_operator;
}

static size_t fix_col_right_motion_cursor(size_t cursor_col, size_t col, Motion motion, size_t count, Line line) {
    if (cursor_col == col) {
        return col;
    }

    switch (motion) {
        case MOT_RIGHT:
            if (col - cursor_col == count) {
                return col - 1;
            }
            return col;
        case MOT_WORD:
            if (col == line.length - 1) {
                if (!is_word_split(line.chars[col - 1])) {
                    return col;
                }
            }
            return col - 1;
        case MOT_BIG_WORD:
            if (col == line.length - 1) {
                if (!is_big_word_split(line.chars[col - 1])) {
                    return col;
                }
            }
            return col - 1;
        default:
            return col;
    }
}

static void execute_row_motion(Parser* parser, Editor* editor) {
    size_t row = get_motion_row(editor, parser->cmd.motion, parser->cmd.count);

    if (!editor->successful_motion) {
        editor->save_curosr_col = false;
        return;
    }

    if (parser->cmd.operator == OP_NONE) {
        editor->cursor.row = row;
        return;
    }

    switch (parser->cmd.operator) {
        case OP_DELETE:
            delete_row(&editor->buffer, &editor->cursor, row);
            break;
        case OP_CHANGE:
            bool is_last_line = editor->cursor.row == editor->buffer.line_count - 1 || row == editor->buffer.line_count - 1;
            bool is_empty = is_last_line && (editor->cursor.row == 0 || row == 0);
            delete_row(&editor->buffer, &editor->cursor, row);

            editor->mode = INSERT;
            if (is_empty) {
                break;
            }

            if (is_last_line) {
                append_line(&editor->buffer, &editor->cursor);
            } else {
                prepend_line(&editor->buffer, &editor->cursor);
            }
            break;
        case OP_YANK:
            // TODO: implement yank row motion
            break;
        default:
            ERROR("Wrong operator");
    }
}

static void execute_col_left_motion(Parser* parser, Editor* editor) {
    size_t col = get_motion_col_left(editor, parser->cmd.motion, parser->cmd.count, parser->cmd.argument);

    if (!editor->successful_motion) {
        editor->save_curosr_col = false;
        return;
    }

    if (parser->cmd.operator == OP_NONE) {
        editor->cursor.col = col;
        return;
    }

    switch (parser->cmd.operator) {
        case OP_DELETE:
            delete_col_left(&editor->buffer, &editor->cursor, col);
            break;
        case OP_CHANGE:
            delete_col_left(&editor->buffer, &editor->cursor, col);
            editor->mode = INSERT;
            break;
        case OP_YANK:
            // TODO: implement yank col left motion
            break;
        default:
            ERROR("Wrong operator");
    }
}

static void execute_col_right_motion(Parser* parser, Editor* editor) {
    size_t col = get_motion_col_right(editor, parser->cmd.motion, parser->cmd.count, parser->cmd.argument);

    if (!editor->successful_motion) {
        editor->save_curosr_col = false;
        return;
    }

    if (parser->cmd.operator == OP_NONE) {
        editor->cursor.col = col;
        return;
    }

    col = fix_col_right_motion_cursor(editor->cursor.col, col, parser->cmd.motion, parser->cmd.count, editor->buffer.lines[editor->cursor.row]);

    switch (parser->cmd.operator) {
        case OP_DELETE:
            delete_col_right(&editor->buffer, &editor->cursor, col);
            break;
        case OP_CHANGE:
            bool is_end_of_line = col == editor->buffer.lines[editor->cursor.row].length - 1;
            delete_col_right(&editor->buffer, &editor->cursor, col);

            editor->mode = INSERT;
            if (is_end_of_line && editor->buffer.lines[editor->cursor.row].length > 0) {
                editor->cursor.col++;
            }
            break;
        case OP_YANK:
            // TODO: implement yank col left motion
            break;
        default:
            ERROR("Wrong operator");
    }
}

static void execute_position_motion(Parser* parser, Editor* editor) {
    Position position = get_motion_position(editor, parser->cmd.motion, parser->cmd.count);

    if (!editor->successful_motion) {
        editor->save_curosr_col = false;
        return;
    }

    if (parser->cmd.operator == OP_NONE) {
        editor->cursor = position;
        return;
    }

    switch (parser->cmd.operator) {
        case OP_DELETE:
            break;
        case OP_CHANGE:
            break;
        case OP_YANK:
            break;
        default:
            ERROR("Wrong operator");
    }
}

static void execute_normal_mode(Parser* parser, Editor* editor) {
    fix_count(parser);

    editor->successful_motion = true;

    if (parser->cmd.operator == OP_REPLACE) {
        replace_char(&editor->buffer, &editor->cursor, parser->cmd.count, parser->cmd.argument);
        editor->saved = false;
        return;
    }

    if (parser->cmd.operator == OP_NONE) {
        if (is_movement_to_execute(editor, parser->cmd.motion, parser->cmd.count)) {
            return;
        }
    }

    editor->saved = false;
    switch (get_motion_type(parser->cmd.motion)) {
        case MOT_TYPE_ROW:
            execute_row_motion(parser, editor);
            break;
        case MOT_TYPE_COL_LEFT:
            execute_col_left_motion(parser, editor);
            break;
        case MOT_TYPE_COL_RIGHT:
            execute_col_right_motion(parser, editor);
            break;
        case MOT_TYPE_POSITION:
            execute_position_motion(parser, editor);
            break;
        default:
            ERROR("Wrong motion type");
    }
}

static void execute_and_init(Parser* parser, Editor* editor) {
    execute_normal_mode(parser, editor);
    parser_init(parser);
}

static void wrong_input(Parser* parser, Editor* editor) {
    editor->save_curosr_col = false;
    parser_init(parser);
}

static inline void append_digit(size_t* count, char key) {
    size_t digit = (size_t)(key - '0');

    if (*count <= (SIZE_MAX - digit) / 10)
        *count = *count * 10 + digit;
}

void parse_normal_mode(Parser* parser, Editor* editor, int key) {
    if (parser->cmd.motion != MOT_NONE) {
        if (parser->cmd.motion == MOT_FIRST_LINE) {
            if (key != 'g') {
                wrong_input(parser, editor);
                return;
            }

            execute_and_init(parser, editor);
            return;
        }

        if (!(IS_PRINTABLE(key))) {
            wrong_input(parser, editor);
            return;
        }

        parser->cmd.argument = key;
        execute_and_init(parser, editor);
        return;
    }

    if (parser->cmd.operator == OP_REPLACE) {
        if (!(IS_PRINTABLE(key))) {
            wrong_input(parser, editor);
            return;
        }

        parser->cmd.argument = key;
        execute_and_init(parser, editor);
        return;
    }

    if (key >= '0' && key <= '9') {
        if (parser->state == STATE_NORMAL) {
            if (!(key == '0' && parser->cmd.count == 0)) {
                if (parser->cmd.motion != MOT_NONE) {
                    wrong_input(parser, editor);
                    return;
                }

                append_digit(&parser->cmd.count, key);
                editor->save_curosr_col = false;
                return;
            }
        } else {
            if (!(key == '0' && parser->cmd.count_after_operator == 0)) {
                if (parser->cmd.motion != MOT_NONE) {
                    wrong_input(parser, editor);
                    return;
                }

                append_digit(&parser->cmd.count_after_operator, key);
                editor->save_curosr_col = false;
                return;
            }
        }
    }

    parser->cmd.key_cache.chars[parser->cmd.key_cache.length++] = key;
    parser->cmd.key_cache.chars[parser->cmd.key_cache.length] = '\0';

    bool ready_to_execute = true;
    switch (key) {
        case 'h':
        case LEFT:
            parser->cmd.motion = MOT_LEFT;
            break;
        case 'j':
        case DOWN:
            parser->cmd.motion = MOT_DOWN;
            break;
        case 'k':
        case UP:
            parser->cmd.motion = MOT_UP;
            break;
        case 'l':
        case RIGHT:
            parser->cmd.motion = MOT_RIGHT;
            break;
        case 'w':
            parser->cmd.motion = MOT_WORD;
            break;
        case 'W':
            parser->cmd.motion = MOT_BIG_WORD;
            break;
        case 'e':
            parser->cmd.motion = MOT_END_WORD;
            break;
        case 'E':
            parser->cmd.motion = MOT_END_BIG_WORD;
            break;
        case 'b':
            parser->cmd.motion = MOT_BACKWARD_WORD;
            break;
        case 'B':
            parser->cmd.motion = MOT_BACKWARD_BIG_WORD;
            break;
        case '0':
            parser->cmd.motion = MOT_START_OF_LINE;
            break;
        case '^':
            parser->cmd.motion = MOT_FIRST_NON_BLANK_CHAR_OF_LINE;
            break;
        case '$':
            parser->cmd.motion = MOT_END_OF_LINE;
            break;
        case 'G':
            parser->cmd.motion = MOT_LAST_LINE;
            break;
        case '}':
            parser->cmd.motion = MOT_NEXT_PARAGRAPH;
            break;
        case '{':
            parser->cmd.motion = MOT_PREVIOUS_PARAGRAPH;
            break;
        case '%':
            parser->cmd.motion = MOT_MATCHING_CHAR;
            break;

        case 'g':
            parser->cmd.motion = MOT_FIRST_LINE;
            return;
        case 'f':
            parser->cmd.motion = MOT_NEXT_OCCURRENCE_OF_CHAR;
            return;
        case 't':
            parser->cmd.motion = MOT_BEFORE_NEXT_OCCURRENCE_OF_CHAR;
            return;
        case 'F':
            parser->cmd.motion = MOT_PREVIOUS_OCCURRENCE_OF_CHAR;
            return;
        case 'T':
            parser->cmd.motion = MOT_AFTER_PREVIOUS_OCCURRENCE_OF_CHAR;
            return;

        default:
            ready_to_execute = false;
            break;
    }

    if (ready_to_execute) {
        execute_and_init(parser, editor);
        return;
    }

    switch (parser->state) {
        case STATE_NORMAL:
            switch (key) {
                case 'd':
                    parser->state = STATE_OPERATOR_PENDING;
                    parser->cmd.operator = OP_DELETE;
                    editor->save_curosr_col = false;
                    return;
                case 'c':
                    parser->state = STATE_OPERATOR_PENDING;
                    parser->cmd.operator = OP_CHANGE;
                    editor->save_curosr_col = false;
                    return;
                case 'y':
                    parser->state = STATE_OPERATOR_PENDING;
                    parser->cmd.operator = OP_YANK;
                    editor->save_curosr_col = false;
                    return;
                case 'r':
                    parser->state = STATE_OPERATOR_PENDING;
                    parser->cmd.operator = OP_REPLACE;
                    editor->save_curosr_col = false;
                    return;
            }
            break;
        case STATE_OPERATOR_PENDING:
            switch (key) {
                case 'd':
                    if (parser->cmd.operator == OP_DELETE) {
                        parser->cmd.motion = MOT_LINE;
                        execute_and_init(parser, editor);
                    } else {
                        wrong_input(parser, editor);
                    }
                    return;
                case 'c':
                    if (parser->cmd.operator == OP_CHANGE) {
                        parser->cmd.motion = MOT_LINE;
                        execute_and_init(parser, editor);
                    } else {
                        wrong_input(parser, editor);
                    }
                    return;
                case 'y':
                    if (parser->cmd.operator == OP_YANK) {
                        parser->cmd.motion = MOT_LINE;
                        execute_and_init(parser, editor);
                    } else {
                        wrong_input(parser, editor);
                    }
                    return;
                default:
                    wrong_input(parser, editor);
                    return;
            }
            break;
    }

    switch (key) {
        case 'i':
            editor->mode = INSERT;
            parser_init(parser);
            return;
        case 'a':
            editor->mode = INSERT;
            insert_arrow_right(editor);
            parser_init(parser);
            return;
        case 'I':
            editor->mode = INSERT;
            editor->cursor.col = get_motion_col(editor, MOT_FIRST_NON_BLANK_CHAR_OF_LINE);
            parser_init(parser);
            return;
        case 'A':
            editor->mode = INSERT;
            editor->cursor.col = get_motion_col(editor, MOT_END_OF_LINE);
            insert_arrow_right(editor);
            parser_init(parser);
            return;
        case 'o':
            editor->mode = INSERT;
            append_line(&editor->buffer, &editor->cursor);
            parser_init(parser);
            return;
        case 'O':
            editor->mode = INSERT;
            prepend_line(&editor->buffer, &editor->cursor);
            parser_init(parser);
            return;
        case 'v':
            editor->mode = VISUAL;
            // TODO: prepare for visual mode
            parser_init(parser);
            return;
        case 'V':
            editor->mode = VISUAL_LINE;
            // TODO: prepare for visual line mode
            parser_init(parser);
            return;
        case 'x':
            parser->cmd.operator = OP_DELETE;
            parser->cmd.motion = MOT_RIGHT;
            execute_and_init(parser, editor);
            return;
        case 'X':
            parser->cmd.operator = OP_DELETE;
            parser->cmd.motion = MOT_LEFT;
            execute_and_init(parser, editor);
            return;
        case 'J':
            if (editor->cursor.row == editor->buffer.line_count - 1) {
                editor->save_curosr_col = false;
            } else {
                editor->cursor.row++;
                size_t next_row_first_col = get_motion_col_left(editor, MOT_FIRST_NON_BLANK_CHAR_OF_LINE, 1, SPACE);
                editor->cursor.row--;

                join_lines(&editor->buffer, &editor->cursor, next_row_first_col);
            }
            parser_init(parser);
            return;
        case ':':
        case '/':
            editor->mode = COMMAND;
            editor->command_line.line[0] = key;
            editor->command_line.line[1] = '\0';
            editor->command_line.cursor_col = 1;
            editor->command_line.length = 1;
            parser_init(parser);
            return;
    }

    wrong_input(parser, editor);
}
