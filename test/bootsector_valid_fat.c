#include <bootsector.h>
#include <stdio.h>
#include "macros.h"

int main()
{
    FILE* file = fopen( "res/fat32.img", "r" );
    require_nonnull( file );

    bootsector_t boot;
    require( "%d", true, bootsector_init( &boot, file ) );

    str_require( "mkdosfs", boot.oem_name );
    str_require( "           ", boot.volume_label );

    require( "%d", 512, boot.bytes_per_sector );
    require( "%d", 1, boot.sectors_per_cluster );
    require( "%d", 32, boot.reserved_sector_count );
    require( "%d", 2, boot.fat_count );
    require( "%d", 0, boot.root_entry_count );
    require( "%d", 1009, boot.fat_sector_count );
    require( "%d", 2, boot.root_cluster );

    return 0;
}
