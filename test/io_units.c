#include <filesystem.h>
#include <io.h>
#include "macros.h"

int main()
{
    test_init();

    filesystem_t fs;
    require( "%d", true, filesystem_init( &fs, "res/fat32.img" ) );

    require( "%d", fs.fat, sector_to_offset( &fs, 0u ) );
    require( "%d", fs.clusters, cluster_to_offset( &fs, 2u ) );
}
