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
typedef struct directory_t  directory_t;

#include <file.h>

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

    /** The current path, as a string, for the prompt. */
    char*           dir;

};

/**
 * A directory listing.
 *
 * [files] is an pointer to a section of unmanaged, untracked
 * heap memory, so please free that after you are done.
 */
struct directory_t
{
    unsigned int    count;

    file_t*         files;
};

/**
 * Initializes the filesystem and opens the file with the given name.
 * This will immediately try to aprse the FAT32 bootsector.
 *
 * Returns if the filesystem was successfully opened.
 */
bool filesystem_init( filesystem_t*, const char* file_name );

/**
 * Lists the files in the current working directory.
 */
directory_t filesystem_list( filesystem_t* );

/**
 * Changes the current working directory to the directory with the
 * given name, `dir_name`.
 *
 * Returns if the current directory was moved. This could fail if
 * no file with the given name exists or if the file was not a
 * directory.
 */
bool filesystem_cd( filesystem_t*, const char* dir_name );

//G:

void filesystem_close(filesystem_t*);

void filesystem_info(filesystem_t*);

void filesystem_stat(filesystem_t*, const char* file_name);

void filesystem_get(filesystem_t*, const char* file_name); 

void filesystem_read(filesystem_t*, int startOffset, int numOfBytes, const char* file_name, FILE* out);

#endif
