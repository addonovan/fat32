#include <file.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _concat(a, b) a##b
#define concat(a, b) _concat(a, b)
#define padding( size ) uint8_t concat(_, __COUNTER__)[ size ]

typedef struct __attribute__((__packed__))
{
    char        name[ 11 ];
    uint8_t     attributes;

    padding( 8 );
    
    uint16_t    cluster_high;

    padding( 4 );

    uint16_t    cluster_low;
    uint32_t    size;

} raw_file_t;

#undef padding
#undef concat
#undef _concat

void file_read( file_t* this, filesystem_t* fs, offset_t start )
{
    // zero ourselves out first
    memset( this, 0, sizeof( *this ) );

    raw_file_t raw;
    if ( !filesystem_read_into( fs, &raw, start, sizeof( raw_file_t ) ) )
    {
        this->_raw = NULL;
        fprintf( stderr, "Failed to read file from image!\n" );
        exit( -1 );
        return;
    }

    // copy the name over
    memcpy( this->name, raw.name, 8 );
    memcpy( this->ext, raw.name + 8, 3 );

    switch ( ( uint8_t ) raw.name[ 0 ] )
    {
        // this file was deleted (or never existed)
        case 0xe5:
        case 0x00:
            this->deleted = true;
            break;

        // the first character is actually 0xe5
        case 0x05:
            this->name[ 0 ] = ( char ) 0xe5;
            break;

        // this is actually a directory
        case 0x2e:
            strncpy( this->name + 0, this->name + 1, sizeof( this->name ) - 2 );
            this->name[ sizeof( this->name ) - 1 ] = '\0';
            break;
    }

    // read the attributes into a more human-friendly manner
    uint8_t attrs = raw.attributes;
    this->attrs         = attrs;
    this->read_only     = ( attrs & 0b00000001 ) != 0;
    this->hidden        = ( attrs & 0b00000010 ) != 0;
    this->system_file   = ( attrs & 0b00000100 ) != 0;
    this->volume_label  = ( attrs & 0b00001000 ) != 0;
    this->subdirectory  = ( attrs & 0b00010000 ) != 0;
    this->archived      = ( attrs & 0b00100000 ) != 0;

    // copy the data onto the heap, and point to it (for later, of course)
    this->_raw = malloc( sizeof( raw_file_t ) );
    memcpy( this->_raw, &raw, sizeof( raw_file_t ) );
}

void file_destroy( file_t* this )
{
    // free our heap memory
    free( this->_raw );
}

bool file_displayable( file_t* this )
{
    if ( this->deleted ) return false;

    return this->read_only
        || this->subdirectory
        || this->archived;
}
