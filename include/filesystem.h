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
    /** The number of files in this directory. */
    unsigned int    count;

    /** An (unmanaged) list of files in this directory. */
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

/**
 * Closes the given file system's file.
 */
void filesystem_close( filesystem_t* );

/**
 * Prints filesystem volume information to `stdout`.
 */
void filesystem_info( filesystem_t* );

/**
 * Prints information and statistics for the file with the given `file_name`
 * in the current working directory.
 * 
 * Returns true if the file existed.
 */
bool filesystem_stat( filesystem_t*, const char* file_name );

/**
 * Reads `file_name` from the current work directory and extracts its
 * contents and writes them to a file with the name `file_name` on the
 * actual filesystem.
 *
 * Returns the final status of the read & write.
 */
bool filesystem_get( filesystem_t*, const char* file_name ); 

/**
 * Reads `file_name` from the current working directory, starting at
 * byte `start_offset` and reading `length` bytes, writing those
 * characters to `out`.
 *
 * If `length` is less than 0, then the entire file will be read.
 *
 * Returns the final status of the read.
 */
bool filesystem_read( 
    filesystem_t*, 
    offset_t start_offset, 
    int length, 
    const char* file_name, 
    FILE* out
);

#endif
