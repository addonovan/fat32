#include <stdio.h>
#include <filesystem.h>
#include <bootsector.h>
#include <file.h>
#include <stdlib.h>

int main()
{
    filesystem_t fs;
    if ( !filesystem_init( &fs, "res/fat32.img" ) )
    {
        fflush( stderr );
        return 1;
    }

    file_t file;
    for ( int i = 0; i < 16; i++ )
    {
        file_init( &file, &fs, fs.clusters + ( 32 * i ) );

        if ( file.status.last )
            break;

        if ( file_displayable( &file ) )
        {
            printf( file.attrs.archive        ? "A" : "-" );
            printf( file.attrs.directory      ? "S" : "-" );
            printf( file.attrs.volume_id      ? "v" : "-" );
            printf( file.attrs.system         ? "s" : "-" );
            printf( file.attrs.hidden         ? "h" : "-" );
            printf( file.attrs.read_only      ? "R" : "-" );

            printf( "   %s %s\n", file.name, file.ext );
        }
    }

    // print contents of bar.txt
    file_init( &file, &fs, fs.clusters );
    printf( "Contents of %s.%s:\n", file.name, file.ext );

    char* buf = calloc( sizeof( char ), 1024 );
    file_read( &file, &buf );
    printf( "%s\n", buf );

    free( buf );

    return 0;
}
