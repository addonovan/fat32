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
 * If a string is only partially matched, all the variables up to the
 * first failure is filled in.
 *
 * Returns the number of filled variables that were matched
 */
int input_matches( input_t*, const char* format, ... );

#endif
