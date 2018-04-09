#ifndef __FILE_H__
#define __FILE_H__

#define RAW_FILE_SIZE 32

#include <stdint.h>

typedef struct file_attrs_t     file_attrs_t;
typedef struct file_t           file_t;

#include <filesystem.h>

/**
 * The attributes of a file. These are parsed
 * from the `attrs` byte of a directory listing.
 */
struct file_attrs_t
{
    
    bool    read_only;
    bool    hidden;
    bool    system;
    bool    volume_id;
    bool    directory;
    bool    archive;

};

/**
 * A file, or directory listing describing a file.
 */
struct file_t
{
    /** The filesystem we're a part of */
    const filesystem_t* fs;

    /** The id of the first cluster containing this file's contents.*/
    cluster_t       cluster_low;

    /** The id of the last cluster containing this file's contents (or 0). */
    cluster_t       cluster_high;

    /** The size of the file (in bytes). */
    uint32_t        size;

    /** A null-terminated copy of the file's name. */
    char            name[ 9 ];

    /** A null-terminated copy of the file's extension. */
    char            ext[ 4 ];

    /** Human-readable attributes of the file. */
    file_attrs_t    attrs;

    /** If this file has been deleted (or simply never existed) */
    bool            deleted;

};

/**
 * Initializes a file by reading its description at the given
 * offset.
 */
void file_init( file_t*, const filesystem_t* fs, offset_t start );

/**
 * Reads the next cluster of the file into the buffer (make sure
 * that this is at least the size of one cluster, see `io_clalloc`).
 * 
 * Returns if the read was successful.
 */
bool file_read( file_t*, void* buf );

/**
 * Determines if, based on the file's attributes and status, the
 * file should be displayed with the `ls` command.
 */
bool file_displayable( file_t* );

/**
 * Checks if this file's name matches the given `name` and `ext`.
 * This is case-insensitive.
 *
 * If `ext` is `NULL`, then the file is treated as a directory.
 */
bool file_name( file_t*, const char* name, const char* ext );

#endif
