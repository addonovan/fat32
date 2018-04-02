#include <stdio.h>
#include <bootsector.h>

int main()
{
    bootsector_t bootsector;
    FILE* file = fopen( "res/fat32.img", "r" );

    bootsector_init( &bootsector, file );

    printf( "oem_name               = %s\n", bootsector.oem_name );
    printf( "bytes_per_sector       = %d\n", bootsector.bytes_per_sector );
    printf( "sectors_per_cluster    = %d\n", bootsector.sectors_per_cluster );
    printf( "reserved_sector_count  = %d\n", bootsector.reserved_sector_count );
    printf( "fat_count              = %d\n", bootsector.fat_count );
    printf( "root_entry_count       = %d\n", bootsector.root_entry_count );
    printf( "fat_sector_count       = %d\n", bootsector.fat_sector_count );
    printf( "root_cluster           = %d\n", bootsector.root_cluster );
    printf( "volume_label           = %s\n", bootsector.volume_label );

    return 0;
}
