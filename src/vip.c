#include "vip.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "draw.h"
#include "input.h"
#include "parser.h"

static Editor editor;
static Parser parser;

void cleanup(void) {
    free_editor(&editor);

    disable_raw_mode();
    exit_alt_screen();
}

void run_vip(char* filename) {
    enter_alt_screen();
    enable_raw_mode();

    editor_init(&editor, filename);

    signal(SIGWINCH, handle_resize);
    update_window_size();

    while (editor.mode != EXIT) {
        int key = read_key();

        editor.in_start = false;
        editor.save_curosr_col = true;

        handle_key(&parser, &editor, key);

        fix_text_position(editor.cursor, &editor.text, editor.buffer.line_count);

        if (editor.save_curosr_col) {
            editor.prev_cursor_col = editor.cursor.col;
        }

        if (editor.mode == INSERT) {
            snprintf(editor.command_line.line, sizeof(editor.command_line.line), INSERT_STATUS_LINE);
        } else if (editor.mode == VISUAL) {
            snprintf(editor.command_line.line, sizeof(editor.command_line.line), VISUAL_STATUS_LINE);
        } else if (editor.mode == VISUAL_LINE) {
            snprintf(editor.command_line.line, sizeof(editor.command_line.line), VISUAL_LINE_STATUS_LINE);
        }

        draw_window();

        refresh_window();
    }

    cleanup();
}

Line* get_line(size_t row) {
    return &editor.buffer.lines[row];
}

char* get_command_line(void) {
    return editor.command_line.line;
}

size_t get_command_line_cursor_col(void) {
    return editor.command_line.cursor_col;
}

char* get_key_cache_string(void) {
    char tmp[64];
    if (parser.cmd.count == 0) {
        if (parser.cmd.count_after_operator == 0) {
            snprintf(tmp, sizeof(tmp), "%s", parser.cmd.key_cache.chars);
        } else {
            snprintf(tmp, sizeof(tmp), "%c%zu%s", parser.cmd.key_cache.chars[0], parser.cmd.count_after_operator, parser.cmd.key_cache.chars + 1);
        }
    } else {
        if (parser.cmd.count_after_operator == 0) {
            snprintf(tmp, sizeof(tmp), "%zu%s", parser.cmd.count, parser.cmd.key_cache.chars);
        } else {
            snprintf(tmp, sizeof(tmp), "%zu%c%zu%s", parser.cmd.count, parser.cmd.key_cache.chars[0], parser.cmd.count_after_operator, parser.cmd.key_cache.chars + 1);
        }
    }

    memmove(editor.key_cache_string, tmp, KEY_CACHE_STRING_SIZE);
    editor.key_cache_string[KEY_CACHE_STRING_SIZE] = '\0';

    return editor.key_cache_string;
}

char* get_cursor_position_string(void) {
    if (editor.cursor.col == 0 && get_line(editor.cursor.row)->chars[0] == '\0') {
        snprintf(editor.cursor_position_string, sizeof(editor.cursor_position_string), "%zu,0-1", editor.cursor.row + 1);
    } else {
        snprintf(editor.cursor_position_string, sizeof(editor.cursor_position_string), "%zu,%zu", editor.cursor.row + 1, editor.cursor.col + 1);
    }

    return editor.cursor_position_string;
}

size_t get_line_count(void) {
    return editor.buffer.line_count;
}

Position get_cursor_position(void) {
    return editor.cursor;
}

Position get_text_position(void) {
    return editor.text;
}

bool has_number(void) {
    return editor.number;
}

bool has_relative_number(void) {
    return editor.relative_number;
}

bool is_in_start(void) {
    return editor.in_start;
}

Mode get_mode(void) {
    return editor.mode;
}
