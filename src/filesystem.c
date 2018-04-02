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

    return true;
}

