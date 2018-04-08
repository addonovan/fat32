#include <io.h>
#include <filesystem.h>
#include <file.h>
#include "macros.h"

int main()
{
    filesystem_t fs;
    require( "%d", true, filesystem_init( &fs, "res/fat32.img" ) );

    // grab the BAR.TXT file from the root directory
    file_t file;
    file_init( &file, &fs, fs.clusters );

    str_require( "BAR", file.name );
    str_require( "TXT", file.ext );

    // read the first cluster of the file
    char* cluster = io_clalloc( &fs, 1 );
    require( "%d", true, file_read( &file, cluster ) );
    str_require( "5 6 7 8\n", cluster );
    
    // clean up after ourselves
    io_free( cluster );

    return 0;
}
