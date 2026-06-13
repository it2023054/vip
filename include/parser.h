#pragma once

#include <stddef.h>

#include "editor.h"
#include "motion.h"
#include "operator.h"

#define KEY_CACHE_SIZE 3

typedef struct {
    char chars[KEY_CACHE_SIZE + 1];
    int length;
} KeyCache;

typedef struct {
    size_t count;
    size_t count_after_operator;
    Operator operator;
    Motion motion;
    char argument;
    KeyCache key_cache;
} Command;

typedef enum {
    STATE_NORMAL,
    STATE_OPERATOR_PENDING
} ParserState;

typedef struct {
    ParserState state;
    Command cmd;
} Parser;

void parser_init(Parser* parser);
void handle_key(Parser* parser, Editor* editor, int key);
