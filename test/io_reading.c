#include <io.h>
#include <filesystem.h>
#include "macros.h"

int main()
{
    filesystem_t fs;
    filesystem_init( &fs, "res/fat32.img" );

    void* sector = io_clalloc( &fs, 1 );
    require( "%d", true, io_read_cluster( &fs, 17, sector ) );
    str_require( "5 6 7 8\n", ( char* ) sector );
    io_free( sector );

    return 0;
}
