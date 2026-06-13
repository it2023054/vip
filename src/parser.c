#include "parser.h"

#include <stdbool.h>

#include "command.h"
#include "editor.h"
#include "insert.h"
#include "normal.h"
#include "visual.h"
#include "visual_line.h"

void parser_init(Parser* parser) {
    parser->state = STATE_NORMAL;
    parser->cmd.count = 0;
    parser->cmd.count_after_operator = 0;
    parser->cmd.operator = OP_NONE;
    parser->cmd.motion = MOT_NONE;
    parser->cmd.argument = '\0';
    parser->cmd.key_cache.chars[0] = '\0';
    parser->cmd.key_cache.length = 0;
}

void handle_key(Parser* parser, Editor* editor, int key) {
    switch (editor->mode) {
        case NORMAL:
            parse_normal_mode(parser, editor, key);
            break;
        case INSERT:
            parse_insert_mode(editor, key);
            break;
        case VISUAL:
            parse_visual_mode(parser, editor, key);
            break;
        case VISUAL_LINE:
            parse_visual_line_mode(parser, editor, key);
            break;
        case COMMAND:
            parse_command_mode(editor, key);
            break;
        case EXIT:
            break;
    }
}
