#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <stdio.h>
#include <bootsector.h>
#include <stdbool.h>
#include <stdint.h>

/** An offset in the file system. Our version of a pointer, really. */
typedef uint32_t    offset_t;
typedef uint32_t    sector_t;
typedef uint32_t    cluster_t;

typedef struct
{
    FILE*           _file;

    bootsector_t    _boot;

    offset_t        fat;
    offset_t        clusters;

} filesystem_t;

/**
 * Initializes the filesystem and opens the file with the given name.
 * This will immediately try to aprse the FAT32 bootsector.
 *
 * Returns if the filesystem was successfully opened.
 */
bool filesystem_init( filesystem_t*, const char* file_name );

#endif
