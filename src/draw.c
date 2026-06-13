#include "draw.h"

#include <stdio.h>
#include <string.h>

#include "colors.h"
#include "vip.h"

#define SCROLL_OFF 8

void enter_alt_screen(void) {
    printf("\033[?1049h");
    fflush(stdout);
}

void exit_alt_screen(void) {
    printf("\033[?1049l");
    fflush(stdout);
}

void move_cursor(Position pos) {
    printf("\033[%zu;%zuH", pos.row + 1, pos.col + 1);
}

static size_t digit_count(size_t n) {
    size_t count = 0;
    while (n > 0) {
        n /= 10;
        count++;
    }

    return count;
}

static size_t get_number_width(size_t line_count) {
    size_t number_width = has_number() || has_relative_number() ? 3 : 0;

    if (has_number()) {
        if (line_count >= 1000) {
            number_width = digit_count(line_count);
        }
    }

    return number_width;
}

static void fix_text_row(Position cursor, Position* text, const size_t line_count, size_t max_row) {
    if (cursor.row < text->row + SCROLL_OFF) {
        if (cursor.row < SCROLL_OFF) {
            text->row = 0;
        } else {
            text->row = cursor.row - SCROLL_OFF;
        }
        return;
    }

    if (line_count - text->row <= max_row) {
        return;
    }

    if (cursor.row - text->row + SCROLL_OFF > max_row - 1) {
        if (cursor.row + SCROLL_OFF > line_count - 1) {
            text->row = line_count - max_row;
        } else {
            text->row = cursor.row + SCROLL_OFF - max_row + 1;
        }
        return;
    }
}

static void fix_text_col(Position cursor, Position* text, size_t max_col) {
    size_t number_width = get_number_width(get_line_count());
    if (number_width > 0) {
        max_col -= number_width + 1;
    }

    if (cursor.col < text->col) {
        text->col = cursor.col;
    } else if (cursor.col - text->col >= max_col) {
        text->col = cursor.col - max_col + 1;
    }
}

void fix_text_position(Position cursor, Position* text, const size_t line_count) {
    Position max_buffer = get_max_buffer();

    fix_text_row(cursor, text, line_count, max_buffer.row);
    fix_text_col(cursor, text, max_buffer.col);
}

void clean_screen(void) {
    printf("\033[H\033[2J");
}

void clean_line(void) {
    printf("\r\033[2K");
}

static void print_centered_lines(const char* text[], size_t text_length[], size_t n_text) {
    Position max_buffer = get_max_buffer();

    size_t start_row = (max_buffer.row - n_text) / 2;
    for (size_t i = 0; i < n_text; i++) {
        size_t col = (max_buffer.col - text_length[i]) / 2;
        move_cursor((Position){start_row + i, col});
        printf("%s", text[i]);
    }
}

static void draw_tildes(size_t start_row) {
    for (size_t r = start_row; r < get_max_buffer().row; r++) {
        move_cursor((Position){r, 0});
        printf(BLUE "~" RESET);
    }
}

static void draw_start(void) {
    const char* text[] = {
        "VIP - Vi rIPoff",
        "",
        "version idk",
        "by Alexandros Papagrigoriou",
        "and Alexios Lagios",
        "",
        "Vip is open source and freely distributable",
        "https://github.com/it2023054/vip",
        "",
        "type  :q" BLUE "<Enter>" RESET "  to exit"};

    size_t n_text = sizeof(text) / sizeof(text[0]);

    size_t text_length[n_text];
    for (size_t i = 0; i < n_text; i++) {
        text_length[i] = strlen(text[i]);
    }
    text_length[9] -= COLOR_SIZE + RESET_COLOR_SIZE;

    print_centered_lines(text, text_length, n_text);
    draw_tildes(1);
}

static void draw_buffer(void) {
    Position max_buffer = get_max_buffer();
    size_t line_count = get_line_count();
    size_t cursor_row = get_cursor_position().row;

    size_t number_width = get_number_width(line_count);

    for (size_t r = 0; r < max_buffer.row; r++) {
        size_t draw_row = r + get_text_position().row;
        if (draw_row >= line_count) {
            break;
        }

        move_cursor((Position){r, 0});
        if (has_relative_number()) {
            if (draw_row == cursor_row && has_number()) {
                printf(YELLOW "%*zu" RESET " ", -(int)number_width, draw_row + 1);
            } else {
                size_t relative_row = draw_row > cursor_row ? draw_row - cursor_row : cursor_row - draw_row;
                printf(YELLOW "%*zu" RESET " ", (int)number_width, relative_row);
            }
        } else if (has_number()) {
            printf(YELLOW "%*zu" RESET " ", (int)number_width, draw_row + 1);
        }

        Line* line = get_line(draw_row);
        if (line->length <= get_text_position().col) {
            continue;
        }

        printf("%.*s", (int)max_buffer.col - (int)number_width, line->chars + get_text_position().col);
    }

    draw_tildes(line_count - get_text_position().row);
}

static void draw_command(void) {
    move_cursor((Position){get_max_screen().row - 1, 0});
    clean_line();

    printf("%.*s", (int)get_max_screen().col, get_command_line());
}

static void draw_status_line(void) {
    size_t max_row = get_max_screen().row - 1;

    move_cursor((Position){max_row, 0});
    clean_line();

    int max_col = (int)get_max_screen().col - (KEY_CACHE_STRING_SIZE + CURSOR_POSITION_STRING_SIZE + 3);
    printf("%.*s", max_col, get_command_line());

    max_col++;

    move_cursor((Position){max_row, max_col});
    printf("%.*s", KEY_CACHE_STRING_SIZE, get_key_cache_string());

    max_col += KEY_CACHE_STRING_SIZE + 1;

    move_cursor((Position){max_row, max_col});
    printf("%.*s", CURSOR_POSITION_STRING_SIZE, get_cursor_position_string());
}

static void hide_cursor(void) {
    printf("\033[?25l");
}

static void show_cursor(void) {
    printf("\033[?25h");
}

void draw_window(void) {
    hide_cursor();

    clean_screen();

    if (is_in_start()) {
        draw_start();
    } else {
        draw_buffer();
    }

    if (get_mode() == COMMAND) {
        draw_command();
    } else {
        draw_status_line();
    }
}

void refresh_window(void) {
    if (get_mode() == COMMAND) {
        Position command_line_pos = (Position){get_max_screen().row - 1, get_command_line_cursor_col()};
        move_cursor(command_line_pos);
    } else {
        Position buffer_pos = (Position){get_cursor_position().row - get_text_position().row, get_cursor_position().col - get_text_position().col};
        size_t number_width = get_number_width(get_line_count());
        if (number_width > 0) {
            buffer_pos.col += number_width + 1;
        }
        move_cursor(buffer_pos);
    }

    show_cursor();

    fflush(stdout);
}
