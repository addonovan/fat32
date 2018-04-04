#include <filesystem.h>

bool filesystem_init( filesystem_t* this, const char* file_name )
{
    // open the image file for reading
    this->_file = fopen( file_name, "r" );
    if ( this->_file == NULL )
    {
        fprintf( stderr, "Failed to open '%s' for read\n", file_name );
        perror( "fopen()" );
        return false;
    }

    // load the bootsystem/fat32 metadata
    if ( !bootsector_init( &this->_boot, this->_file ) )
    {
        fprintf( stderr, "Failed to read fat32 bootsector!\n" );
        fprintf( stderr, "Are you sure this is a valid fat32 image?\n" );
        return false;
    }

    // calculate some useful sizes we'll need in a bit
    offset_t bps            = this->_boot.bytes_per_sector;
    offset_t size_reserved  = this->_boot.reserved_sector_count * bps;
    offset_t size_fat       = this->_boot.fat_sector_count * bps;

    // the first FAT starts immediately after the reserved section
    this->fat = size_reserved;

    // the fs clusters start after the (identical) FATs
    this->clusters = this->fat + ( size_fat * this->_boot.fat_count );

    return true;
}

