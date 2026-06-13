#pragma once

typedef enum {
    OP_NONE,
    OP_DELETE,
    OP_CHANGE,
    OP_YANK,
    OP_REPLACE
} Operator;
