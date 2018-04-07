#include <file.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>

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

void file_init( file_t* this, const filesystem_t* fs, offset_t start )
{
    // zero ourselves out first
    memset( this, 0, sizeof( *this ) );
    this->fs = fs;

    raw_file_t raw;
    if ( !io_read( fs, start, sizeof( raw_file_t ), &raw ) )
    {
        fprintf( stderr, "Failed to read file from image!\n" );
        exit( -1 );
        return;
    }

    // copy the name over
    memcpy( this->name, raw.name, 8 );
    memcpy( this->ext, raw.name + 8, 3 );

    switch ( ( uint8_t ) raw.name[ 0 ] )
    {
        case 0x00:
            this->status.last = true;
            // intentional fall-through

        case 0xe5:
            this->status.free = true;
            break;

        case 0x05:
            this->name[ 0 ] = ( char ) 0xe5;
            break;
    }

    // read the attributes into a more human-friendly manner
    uint8_t attrs = raw.attributes;
    this->attrs.read_only     = ( attrs & 0b00000001 ) != 0;
    this->attrs.hidden        = ( attrs & 0b00000010 ) != 0;
    this->attrs.system        = ( attrs & 0b00000100 ) != 0;
    this->attrs.volume_id     = ( attrs & 0b00001000 ) != 0;
    this->attrs.directory     = ( attrs & 0b00010000 ) != 0;
    this->attrs.archive       = ( attrs & 0b00100000 ) != 0;

    this->cluster_low = raw.cluster_low;
    this->cluster_high = raw.cluster_high;
    this->size = raw.size;
}

bool file_displayable( file_t* this )
{
    if ( this->status.deleted ) 
        return false;
    
    if ( this->attrs.read_only )
        return true;
    
    if ( this->attrs.directory )
        return true;

    if ( this->attrs.archive )
        return true;

    return false;
}

bool file_read( file_t* this, void* buf )
{
    return io_read_cluster(
        this->fs,
        this->cluster_low,
        buf
    );
}
