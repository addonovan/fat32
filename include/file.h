#ifndef __FILE_H__
#define __FILE_H__

#include <stdint.h>
#include <filesystem.h>

typedef struct 
{
    
    bool    read_only;
    bool    hidden;
    bool    system;
    bool    volume_id;
    bool    directory;
    bool    archive;

} file_attrs_t;

typedef struct
{
    
    bool    deleted;
    bool    last;
    bool    free;

} file_status_t;

typedef struct 
{
    /** The filesystem we're a part of */
    const filesystem_t* fs;

    cluster_t       cluster_low;
    cluster_t       cluster_high;
    uint32_t        size;

    /** A null-terminated copy of the file's name. */
    char            name[ 9 ];

    /** A null-terminated copy of the file's extension. */
    char            ext[ 4 ];

    /** Human-readable attributes of the file. */
    file_attrs_t    attrs;

    /** The status of this directory entry. */
    file_status_t   status;

} file_t;

void file_init( file_t*, const filesystem_t* fs, offset_t start );

bool file_read( file_t*, void* buf );

bool file_displayable( file_t* );

#endif
