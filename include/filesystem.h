#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#include <stdio.h>
#include <bootsector.h>
#include <stdbool.h>
#include <stdint.h>

/** An offset in the file system. Our version of a pointer, really. */
typedef uint32_t offset_t;

typedef struct
{
    FILE*           _file;

    bootsector_t    _boot;

    offset_t        fat;
    offset_t        clusters;

} filesystem_t;

bool filesystem_init( filesystem_t*, const char* file_name  );

bool filesystem_read_into( 
    const filesystem_t*, 
    void* dest,
    offset_t start, 
    offset_t length 
);

#endif
