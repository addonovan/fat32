#include <file.h>
#include <filesystem.h>
#include <stdio.h>
#include <stdlib.h>
#include "macros.h"

int main()
{
    filesystem_t fs;
    require( "%d", true, filesystem_init( &fs, "res/fat32.img" ) );

    require( "%u", 16384u,  fs.fat );
    require( "%u", 1049600u, fs.clusters );

    // test the contents of the root directory
    file_t files[ 16 ];
    unsigned int count = filesystem_list( &fs, files );
    printf( "count = %d\n", count );
    unsigned int i;
    for ( i = 0; i < count; i++ )
    {
        printf( "%d: %s %s\n", i, files[ i ].name, files[ i ].ext );
    }

    return 0;
}
