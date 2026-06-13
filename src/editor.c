#include "editor.h"

#include "fileio.h"

void editor_init(Editor* editor, char* filename) {
    buffer_init(&editor->buffer);
    editor->cursor = (Position){0, 0};
    editor->save_curosr_col = true;
    editor->prev_cursor_col = 0;
    editor->text = (Position){0, 0};
    editor->number = false;
    editor->relative_number = false;
    editor->command_line.line[0] = '\0';
    editor->command_line.cursor_col = 0;
    editor->command_line.length = 0;
    editor->saved = true;
    editor->in_start = filename == NULL;
    editor->mode = NORMAL;
    editor->filename = filename;

    if (filename != NULL) {
        load_file(&editor->buffer, filename);
    }
}

void free_editor(Editor* editor) {
    free_buffer(&editor->buffer);
}
