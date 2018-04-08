#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdio.h>
#include <stdarg.h>

#ifndef INPUT_MAX_LINE_LENGTH
#define INPUT_MAX_LINE_LENGTH 1024
#endif

typedef struct input_t input_t;

struct input_t
{
    char input[ INPUT_MAX_LINE_LENGTH ];
};

/**
 * Reads a line of input from the specified file.
 */
void input_read( input_t*, FILE* stream );

/**
 * Tries to match the read string against the given format specifier.
 * If the string is matched, then the parameters are filled in with
 * their values; otherwise, the values of the parameters are left
 * undefined.
 *
 * Returns if the format matched the given line of input.
 */
int input_matches( input_t*, const char* format, ... );

#endif
