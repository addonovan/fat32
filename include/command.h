#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <filesystem.h>
#include <stdbool.h>

/**
 * Tries to run all of the built in commands.
 *
 * Returns if any of the built-in commands were invoked.
 */
bool try_commands( filesystem_t** fs, int argc, char** argv );

#endif
