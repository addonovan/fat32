#include <file.h>
#include <filesystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "macros.h"

int main()
{
    filesystem_t fs;
    require( "%d", true, filesystem_init( &fs, "res/fat32.img" ) );

    require( "%u", 16384u,  fs.fat );
    require( "%u", 1049600u, fs.clusters );

    // test the contents of the root directory
    directory_t dir = filesystem_list( &fs );

    printf( "count = %d\n", dir.count );
    unsigned int i;
    for ( i = 0; i < dir.count; i++ )
    {
        printf(
            "%2d: %8s %3s (%c%c%c%c%c%c)\n", 
            i,
            dir.files[ i ].name,
            dir.files[ i ].ext,
            dir.files[ i ].attrs.read_only  ? 'R' : '-',
            dir.files[ i ].attrs.hidden     ? 'h' : '-',
            dir.files[ i ].attrs.system     ? 's' : '-',
            dir.files[ i ].attrs.volume_id  ? 'v' : '-',
            dir.files[ i ].attrs.directory  ? 'D' : '-',
            dir.files[ i ].attrs.archive    ? 'A' : '-'
        );
    }
    free( dir.files );

    return 0;
}
