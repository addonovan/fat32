#include <io.h>
#include <filesystem.h>
#include <file.h>
#include "macros.h"

test( "file reader", {
    filesystem_t fs;
    require( "%d", true, filesystem_init( &fs, "res/fat32.img" ) );
    file_t file;

    on( "file entry located", {
        file_init( &file, &fs, fs.clusters );
        str_require( "BAR", file.name );
        str_require( "TXT", file.ext );
        require( "%d", true,  file_name( &file, "BAR.TXT" ) );
        require( "%d", true,  file_name( &file, "bar.TXT" ) );
        require( "%d", true,  file_name( &file, "BAR.txt" ) );
        require( "%d", true,  file_name( &file, "bar.txt" ) );
        require( "%d", false, file_name( &file, "foo.txt" ) );
        require( "%d", false, file_name( &file, "foo" ) );    
    } );

    on( "file data read", {
        file_init( &file, &fs, fs.clusters );
        
        char* buffer = io_clalloc( &fs, 1 );
        require( "%d", true, file_read( &file, buffer ) );
        str_require( "5 6 7 8\n", buffer );
        io_free( buffer );
    } );
} );
