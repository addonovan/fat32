#include <filesystem.h>
#include <stdio.h>
#include "macros.h"

int main()
{
    filesystem_t fs;
    require( "%d", true, filesystem_init( &fs, "res/fat32.img" ) );

    require( "%u", 16384u,  fs.fat );
    require( "%u", 1049600u, fs.clusters );

    return 0;
}
