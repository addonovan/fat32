#include <file.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>

#define _concat(a, b) a##b
#define concat(a, b) _concat(a, b)
#define padding( size ) uint8_t concat(_, __LINE__)[ size ]

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
        case 0xe5:
            this->deleted = true;
            break;

        case 0x05:
            this->name[ 0 ] = ( char ) 0xe5;
            break;
    }

    // now a bit of extra processing on the name/extension,
    // let's remove all of the trailing spaces so it can be treated
    // like a regular string everywhere else
    int j;
    for ( j = 7; j >= 0 && this->name[ j ] == ' '; j-- )
    {
        this->name[ j ] = '\0';
    }
    for ( j = 2; j >= 0 && this->ext[ j ] == ' '; j-- )
    {
        this->ext[ j ] = '\0';
    }

    // read the attributes into a more human-friendly manner
    uint8_t attrs = raw.attributes;
    this->attrs.read_only     = ( attrs & 0x01 ) != 0;
    this->attrs.hidden        = ( attrs & 0x02 ) != 0;
    this->attrs.system        = ( attrs & 0x04 ) != 0;
    this->attrs.volume_id     = ( attrs & 0x08 ) != 0;
    this->attrs.directory     = ( attrs & 0x10 ) != 0;
    this->attrs.archive       = ( attrs & 0x20 ) != 0;

    this->cluster_low = raw.cluster_low;
    this->cluster_high = raw.cluster_high;
    this->size = raw.size;
}

bool file_displayable( file_t* this )
{
    if ( this->deleted || this->attrs.hidden )
        return false;
    
    if ( this->attrs.directory )
        return true;

    if ( this->attrs.archive )
        return true;

    return false;
}

bool file_read( file_t* this, void* buff )
{
    /*
    return io_read_cluster(
        this->fs,
        this->cluster_low,
        buf
    );
    */

    cluster_t cluster = this->cluster_low;

    do
    {
        if(!io_read_cluster(this->fs, cluster, buff))
        {
            return false;
        }

        buff+=(this->fs->_boot.sectors_per_cluster*this->fs->_boot.bytes_per_sector);

    }
    while(io_next_cluster(this->fs, &cluster));

    return true;

}

/**
 * Copies the string and converts it to uppercase. This new string
 * needs to be free'd manually, else you'll be leaking memory.
 */
char* to_uppercase( const char* src )
{
    char* copy = strdup( src );

    char* current = copy;
    while ( *current != '\0' )
    {
        if ( *current >= 'a' && *current <= 'z' )
        {
            *current -= ( 'a' - 'A' );
        }
        current++;
    }

    return copy;
}

bool file_name( file_t* this, const char* name )
{
    char* name_cpy = to_uppercase( name );

    const char* ext = NULL;
    // ignore special files for tokenization
    if ( strcmp( "..", name ) != 0 && strcmp( ".", name ) != 0 )
    {
        strtok( name_cpy, "." );
        ext = strtok( NULL, "." );
    }

    bool status;

    // convert the name to uppercase and check if they're the same
    status = strcmp( this->name, name_cpy ) == 0;

    // if the name doesn't match, just give up
    if ( !status )
    {
        free( name_cpy );
        return false;
    }

    // => name matches

    // if we have no extension, then we just need to check if we're a directory
    if ( ext == NULL )
    {
        free( name_cpy );
        return this->attrs.directory;
    }

    // convert the extension to uppercase and check if they're the same
    status = strcmp( this->ext, ext ) == 0;

    // => name and extensions matches (we're good!)
    free( name_cpy );
    return status;
}
