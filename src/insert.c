#include "insert.h"

#include "keys.h"
#include "movement.h"

void parse_insert_mode(Editor* editor, int key) {
    switch (key) {
        case ESC:
        case CTRL_F:
            insert_arrow_left(editor);
            editor->mode = NORMAL;
            editor->command_line.line[0] = '\0';
            break;
        case TAB:
            insert_string(&editor->buffer, &editor->cursor, "    ");
            break;
        case ENTER:
            insert_enter(&editor->buffer, &editor->cursor);
            break;
        case BACKSPACE:
            insert_backspace(&editor->buffer, &editor->cursor);
            break;
        case LEFT:
            insert_arrow_left(editor);
            break;
        case UP:
            insert_arrow_up(editor);
            break;
        case DOWN:
            insert_arrow_down(editor);
            break;
        case RIGHT:
            insert_arrow_right(editor);
            break;
        default:
            if (IS_PRINTABLE(key)) {
                insert_char(&editor->buffer, &editor->cursor, key);
                editor->saved = false;
            }
            break;
    }
}
