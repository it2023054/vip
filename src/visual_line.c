#include "visual_line.h"

#include "keys.h"

static void fix_count(Parser* parser) {
    if (parser->cmd.count == 0) {
        parser->cmd.count = 1;
    }
}

static void execute_visual_line_mode(Parser* parser, Editor* editor) {
    fix_count(parser);

    editor->successful_motion = true;

    // TODO: implement visual line movements and execute them or update the visual selection
    (void)editor;
}

static void execute_and_init(Parser* parser, Editor* editor) {
    execute_visual_line_mode(parser, editor);
    parser_init(parser);
}

void parse_visual_line_mode(Parser* parser, Editor* editor, int key) {
    // TODO: implement visual line mode parser

    switch (key) {
        case ESC:
        case CTRL_F:
            editor->mode = NORMAL;
            editor->command_line.line[0] = '\0';
            break;
        default:
            execute_and_init(parser, editor);
            break;
    }
}
