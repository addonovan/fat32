#include <bootsector.h>
#include <assert.h>

void bootsector_init( bootsector_t* this, FILE* file )
{
    fseek( file, 3, SEEK_SET );
    fread( this->oem_name, 8, 1, file );

    fseek( file, 11, SEEK_SET );
    fread( &this->bytes_per_sector, 2, 1, file );

    fseek( file, 13, SEEK_SET );
    fread( &this->sectors_per_cluster, 1, 1, file );

    fseek( file, 14, SEEK_SET );
    fread( &this->reserved_sector_count, 2, 1, file );

    fseek( file, 16, SEEK_SET );
    fread( &this->fat_count, 1, 1, file );

    fseek( file, 17, SEEK_SET );
    fread( &this->root_entry_count, 2, 1, file );;

    fseek( file, 36, SEEK_SET );
    fread( &this->fat_sector_count, 4, 1, file );

    fseek( file, 44, SEEK_SET );
    fread( &this->root_cluster, 4, 1, file );

    fseek( file, 71, SEEK_SET );
    fread( this->volume_label, 11, 1, file );
}
