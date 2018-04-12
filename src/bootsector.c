#include <bootsector.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

bool bootsector_init( bootsector_t* this, FILE* file )
{
    // this prevents null terminators from happening to show up
    // even if there's a problem in our code.
    memset( this, -1, sizeof( *this ) );

    fseek( file, 3, SEEK_SET );
    if ( fread( this->oem_name, 8, 1, file ) != 1 )
    {
#ifndef TEST
        fprintf( stderr, "Failed to read BS_OEMNAME\n" );
#endif
        return false;
    }
    this->oem_name[ 7 + 1 ] = '\0'; // manually insert null terminator

    fseek( file, 11, SEEK_SET );
    if ( fread( &this->bytes_per_sector, 2, 1, file ) != 1 )
    {
#ifndef TEST
        fprintf( stderr, "Failed to read BPB_BytsPerSec\n" );
#endif
        return false;
    }

    fseek( file, 13, SEEK_SET );
    if ( fread( &this->sectors_per_cluster, 1, 1, file ) != 1 )
    {
#ifndef TEST
        fprintf( stderr, "Failed to read BPB_SecPerClus\n" );
#endif
        return false;
    }

    fseek( file, 14, SEEK_SET );
    if ( fread( &this->reserved_sector_count, 2, 1, file ) != 1 )
    {
#ifndef TEST
        fprintf( stderr, "Failed to read BPB_RsvdSecCnt\n" );
#endif
        return false;
    }

    fseek( file, 16, SEEK_SET );
    if ( fread( &this->fat_count, 1, 1, file ) != 1 )
    {
#ifndef TEST
        fprintf( stderr, "Failed to read BPB_NumFATs\n" );
#endif
        return false;
    }
    if ( this->fat_count != 2 )
    {
#ifndef TEST
        fprintf( stderr, "BPB_NumFATs != 2\n" );
#endif
        return false;
    }

    fseek( file, 17, SEEK_SET );
    if ( fread( &this->root_entry_count, 2, 1, file ) != 1 )
    {
#ifndef TEST
        fprintf( stderr, "Failed to read BPB_RootEntCnt\n" );
#endif
        return false;
    }

    fseek( file, 36, SEEK_SET );
    if ( fread( &this->fat_sector_count, 4, 1, file ) != 1 )
    {
#ifndef TEST
        fprintf( stderr, "Failed to read BPB_FATSz32\n" );
#endif
        return false;
    }

    fseek( file, 44, SEEK_SET );
    if ( fread( &this->root_cluster, 4, 1, file ) != 1 )
    {
#ifndef TEST
        fprintf( stderr, "Failed to read BPB_RootClus\n" );
#endif
        return false;
    }

    fseek( file, 71, SEEK_SET );
    if ( fread( this->volume_label, 11, 1, file ) != 1 )
    {
#ifndef TEST
        fprintf( stderr, "Failed to read BS_VolLab\n" );
#endif
        return false;
    }
    this->volume_label[ 10 + 1 ] = '\0'; // manually insert null terminator

    // replace all trailing spaces with null terminators
    // this makes it easier to just treat it as a regular
    // C string later
    for ( int i = 10; i >= 0 && this->volume_label[ i ] == ' '; i-- )
    {
        this->volume_label[ i ] = '\0';
    }

    return true;
}
