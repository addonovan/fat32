#include <filesystem.h>
#include <io.h>
#include "macros.h"

int main()
{
    filesystem_t fs;
    filesystem_init( &fs, "res/fat32.img" );

#ifdef MEM_DEBUG
    unsigned int* block = io_alloc( &fs, 1 );
    require( "%u", 0u, block[ -1 ] );
    io_free( block );

    block = io_alloc( &fs, 1 );
    require( "%u", 1u, block[ -1 ] );
    io_free( block );
#endif
}
