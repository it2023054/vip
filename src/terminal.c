#include "terminal.h"

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "draw.h"

static struct termios original;

static Position max_screen;

void enable_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &original);

    struct termios raw = original;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_iflag &= ~(IXON | ICRNL);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}

void update_window_size(void) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    max_screen.row = w.ws_row;
    max_screen.col = w.ws_col;

    draw_window();

    refresh_window();
}

Position get_max_screen(void) {
    return max_screen;
}

Position get_max_buffer(void) {
    return (Position){max_screen.row - 1, max_screen.col};
}
