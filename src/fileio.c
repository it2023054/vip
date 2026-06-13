#include "fileio.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "keys.h"

void save_file(Buffer* buffer, const char* filename, size_t* lines_saved, size_t* bytes_saved) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        ERROR("File cannot be saved");
    }

    *lines_saved = 0;
    *bytes_saved = 0;

    for (size_t i = 0; i < buffer->line_count; i++) {
        Line* line = &buffer->lines[i];
        if (line->length > 0) {
            *bytes_saved += fwrite(line->chars, sizeof(char), line->length, file);
        }

        fputc('\n', file);
        (*bytes_saved)++;

        (*lines_saved)++;
    }

    fclose(file);
}

void load_file(Buffer* buffer, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        if (errno == ENOENT) {
            return;
        }

        ERROR("File cannot be opened");
    }

    char line[512];
    char expanded[512];
    while (fgets(line, sizeof(line), file)) {
        int len = 0;
        for (int i = 0; line[i] != '\n' && line[i] != '\0'; i++) {
            if (line[i] == '\t') {
                for (int k = 0; k < 4; k++) {
                    expanded[len++] = ' ';
                }
            } else {
                if (!(IS_PRINTABLE(line[i]))) {
                    char msg[128];
                    snprintf(msg, sizeof(msg), "Non-printable character in file '%s' (row:%zu, col:%d)", filename, buffer->line_count, i + 1);
                    ERROR(msg);
                }
                expanded[len++] = line[i];
            }
        }
        expanded[len] = '\0';

        insert_string(buffer, &(Position){buffer->line_count - 1, 0}, expanded);
        append_line(buffer, &(Position){buffer->line_count - 1, 0});
    }

    fclose(file);

    delete_row(buffer, &(Position){buffer->line_count - 1, 0}, buffer->line_count - 1);
}
