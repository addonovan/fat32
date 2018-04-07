#include <filesystem.h>
#include <assert.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>
#include <file.h>

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

    // we start off in the root directory
    this->cwd = 2; // for some reason 2 is actually the root directory???

    return true;
}

unsigned int filesystem_list( filesystem_t* this, file_t* files )
{
    offset_t position = cluster_to_offset( this, this->cwd );

    int count = 0;
    int i;
    for ( i = 0; i < 16; i++ )
    {
        file_init( files + i, this, position );

        // if this file is displayable, we'll move to the next one
        if ( file_displayable( files + count ) )
        {
            count++;
            position += 32; // TODO get rid of magic number
        }
    }

    return count;
}
