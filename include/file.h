#ifndef __FILE_H__
#define __FILE_H__

#include <stdint.h>
#include <filesystem.h>

typedef struct 
{
    /** raw data read from the fat32 volume */
    void*   _raw;

    // properties

    /** A null-terminated copy of the name read from the image */
    char    name[ 9 ];
    char    ext[ 4 ];

    int8_t  attrs;

    // derived attributes
    bool    read_only;      // attr: 0b0000 0001
    bool    hidden;         // attr: 0b0000 0010
    bool    system_file;    // attr: 0b0000 0100
    bool    volume_label;   // attr: 0b0000 1000
    bool    subdirectory;   // attr: 0b0001 0000
    bool    archived;       // attr: 0b0010 0000

    bool    directory;      // name[ 0 ] == 0x2e
    bool    deleted;        // name[ 0 ] == 0xe5

} file_t;

void file_read( file_t*, filesystem_t* fs, offset_t start );

void file_destroy( file_t* );

bool file_displayable( file_t* );

#endif
