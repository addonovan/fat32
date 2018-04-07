#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <stdio.h>
#include <bootsector.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * An offset into the file. Just a raw position in the 
 * filesystem image.
 */
typedef uint32_t    offset_t;

/** 
 * A sector in the file. Numbering begins at the first
 * sector in the first FAT.
 */
typedef uint32_t    sector_t;

/**
 * A cluster in the FAT file. Numbering begins at the first
 * cluster in the data section.
 */
typedef uint32_t    cluster_t;

typedef struct filesystem_t filesystem_t;

/**
 * A structure representing a FAT32 filesystem.
 */
struct filesystem_t
{
    /** A file handle for the filesystem image. */
    FILE*           _file;

    /** The bootsector read out of the file system. */
    bootsector_t    _boot;

    /** The offset at which the first FAT begins. */
    offset_t        fat;

    /** The offset at which the first cluster begins. */
    offset_t        clusters;

    /** The current working directory. */
    cluster_t       cwd;

};

#include <file.h>

/**
 * Initializes the filesystem and opens the file with the given name.
 * This will immediately try to aprse the FAT32 bootsector.
 *
 * Returns if the filesystem was successfully opened.
 */
bool filesystem_init( filesystem_t*, const char* file_name );

/**
 * Lists the files in the current working directory. This will write
 * up to 16 files into the `files` parameter, but return the number
 * of files actually written.
 */
unsigned int filesystem_list( filesystem_t*, file_t* files );

#endif
