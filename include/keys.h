#pragma once

#define CTRL_F 6
#define TAB 9
#define ENTER 13
#define ESC 27
#define SPACE 32
#define BACKSPACE 127

#define UP 1000
#define DOWN 1001
#define RIGHT 1002
#define LEFT 1003

#define IS_PRINTABLE(c) c >= 32 && c <= 126
#define IS_DIGIT(c) c >= 48 && c <= 57
#define IS_UPPER_CASE_CHAR(c) c >= 65 && c <= 90
#define IS_LOWER_CASE_CHAR(c) c >= 97 && c <= 122

#define IS_LEFT_PAIR(c) c == '(' || c == '{' || c == '['
#define IS_RIGHT_PAIR(c) c == ')' || c == '}' || c == ']'

#define GET_LEFT_PAIR(c) c == '('   ? ')' \
                         : c == '{' ? '}' \
                         : c == '[' ? ']' \
                                    : '\0'

#define GET_RIGHT_PAIR(c) c == ')'   ? '(' \
                          : c == '}' ? '{' \
                          : c == ']' ? '[' \
                                     : '\0'
