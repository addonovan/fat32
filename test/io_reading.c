#include <io.h>
#include <filesystem.h>
#include "macros.h"

int main()
{
    filesystem_t fs;
    require( "%d", true, filesystem_init( &fs, "res/fat32.img" ) );

    char* cluster = ( char* ) io_clalloc( &fs, 1 );
    require( "%d", true, io_read_cluster( &fs, 17, cluster ) );
    str_require( "5 6 7 8\n", cluster );
    io_free( cluster );

    return 0;
}
