#include <stdio.h>
#include <filesystem.h>
#include <bootsector.h>
#include <file.h>

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
        file_read( &file, &fs, fs.clusters + ( 32 * i ) );

        printf( "[%2d] %s %s\n", i, file.name, file.ext );

        file_destroy( &file );
    }

    return 0;
}
