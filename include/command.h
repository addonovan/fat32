#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <filesystem.h>
#include <stdbool.h>

bool try_commands( filesystem_t** fs, int argc, char** argv );

#endif
