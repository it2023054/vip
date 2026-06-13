#pragma once

#ifdef NDEBUG
#define ERROR(msg) error(msg, NULL, 0, NULL)
#else
#define ERROR(msg) error(msg, __FILE__, __LINE__, __func__)
#endif

void error(const char* msg, const char* file, int line, const char* func);
