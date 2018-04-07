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
        printf( 
            "%2d: %8s %3s (%c%c%c%c%c%c)\n", 
            i, 
            files[ i ].name, 
            files[ i ].ext,
            files[ i ].attrs.read_only  ? 'R' : '-',
            files[ i ].attrs.hidden     ? 'h' : '-',
            files[ i ].attrs.system     ? 's' : '-',
            files[ i ].attrs.volume_id  ? 'v' : '-',
            files[ i ].attrs.directory  ? 'D' : '-',
            files[ i ].attrs.archive    ? 'A' : '-'
        );
    }

    return 0;
}
