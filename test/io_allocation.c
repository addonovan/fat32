#include <filesystem.h>
#include <io.h>
#include "macros.h"

int main()
{
    test_init();

    filesystem_t fs;
    filesystem_init( &fs, "res/fat32.img" );

#ifdef MEM_DEBUG
    unsigned int* block = io_alloc( &fs, 1 );
    require( "%u", 0u, block[ -1 ] );
    io_free( block );

    block = io_alloc( &fs, 1 );
    require( "%u", 1u, block[ -1 ] );
    io_free( block );

    unsigned int* cluster = io_clalloc( &fs, 1 );
    require( "%u", 2u, cluster[ -1 ] );
    io_free( cluster );

    cluster = io_clalloc( &fs, 1 );
    require( "%u", 3u, cluster[ -1 ] );
    io_free( cluster );
#endif
}
