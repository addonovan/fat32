#include <stdio.h>
#include <filesystem.h>
#include <bootsector.h>
#include <file.h>
#include <stdlib.h>

int main()
{
<<<<<<< HEAD
=======
    filesystem_t fs;
    if ( !filesystem_init( &fs, "res/fat32.img" ) )
    {
        fflush( stderr );
        return 1;
    }

    printf( "oem_name               = %s\n", fs._boot.oem_name );
    printf( "bytes_per_sector       = %d\n", fs._boot.bytes_per_sector );
    printf( "sectors_per_cluster    = %d\n", fs._boot.sectors_per_cluster );
    printf( "reserved_sector_count  = %d\n", fs._boot.reserved_sector_count );
    printf( "fat_count              = %d\n", fs._boot.fat_count );
    printf( "root_entry_count       = %d\n", fs._boot.root_entry_count );
    printf( "fat_sector_count       = %d\n", fs._boot.fat_sector_count );
    printf( "root_cluster           = %d\n", fs._boot.root_cluster );
    printf( "volume_label           = %s\n", fs._boot.volume_label );


>>>>>>> Assigned #1, closed
    return 0;
}
