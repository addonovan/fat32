#include <io.h>

bool io_read_sector(
    const filesystem_t* fs,
    sector_t sector,
    void* dst
)
{
    ( void )( fs );
    ( void )( sector );
    ( void )( dst );
    return false;
}

//
// Memory Allocation
//

void* io_alloc( const filesystem_t* fs, unsigned int size )
{
    unsigned int bytes = ( size * fs->_boot.bytes_per_sector ) + 1;

#ifdef MEM_DEBUG
    static unsigned int uid = 0;
    printf( "Allocating %d sectors for %d\n", size, uid );
    bytes += sizeof( uid ); // precede it with the uid
#endif

    unsigned int* memory = calloc( 1, bytes );

#ifdef MEM_DEBUG
    memory[ 0 ] = uid;
    uid++;
    memory += 1; // move one uint down, hiding the uid
#endif

    return memory;
}

void io_free( void* block )
{
    unsigned int* memory = block;

#ifdef MEM_DEBUG
    memory -= 1;
    unsigned int uid = memory[ 0 ];
    printf( "Deallocating %d\n", uid );
#endif

    free( memory );
}
