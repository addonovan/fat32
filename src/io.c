#include <io.h>

//
// Type conversions
//

offset_t cluster_to_offset( const filesystem_t* fs, cluster_t cluster )
{
    offset_t cluster_off = cluster - 2;
    cluster_off *= fs->_boot.sectors_per_cluster;

    return fs->clusters + ( fs->_boot.bytes_per_sector * cluster_off );
}

offset_t sector_to_offset( const filesystem_t* fs, sector_t sector )
{
    return fs->fat + ( fs->_boot.bytes_per_sector * sector );
}

//
// Filesystem reading
//

bool io_read( 
    const filesystem_t* fs, 
    offset_t start, 
    offset_t length,
    void* dst
)
{
    fseek( fs->_file, start, SEEK_SET );
    return fread( dst, length, 1, fs->_file ) == 1;
}

bool io_read_sector(
    const filesystem_t* fs,
    sector_t sector,
    void* dst
)
{
    offset_t start  = sector_to_offset( fs, sector );
    offset_t length = fs->_boot.bytes_per_sector;

    return io_read( fs, start, length, dst );
}

bool io_read_cluster(
    const filesystem_t* fs,
    cluster_t cluster,
    void* dst
)
{
    offset_t start  = cluster_to_offset( fs, cluster );
    offset_t length =  fs->_boot.bytes_per_sector * fs->_boot.sectors_per_cluster;

    return io_read( fs, start, length, dst );
}

bool io_next_cluster( const filesystem_t* fs, cluster_t* cluster )
{
    offset_t fat_offset = fs->fat + ( *cluster * 4 );

    // read the new cluster value from the FAT
    int16_t new_cluster = -1;
    if ( !io_read( fs, fat_offset, 2, &new_cluster ) )
    {
        // read failed, so I guess there's no next cluster
        return false;
    }

    // no successor cluster
    if ( new_cluster < 0 )
    {
        return false;
    }

    // update the value of the cluster
    *cluster = new_cluster;
    return true;
}

//
// Memory Allocation
//

#ifdef MEM_DEBUG
unsigned int uid = 0;
#endif

void* io_alloc( const filesystem_t* fs, unsigned int size )
{
    unsigned int bytes = ( size * fs->_boot.bytes_per_sector ) + 1;

#ifdef MEM_DEBUG
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

void* io_clalloc( const filesystem_t* fs, unsigned int size )
{
    unsigned int bytes = 1;
    bytes += size * fs->_boot.bytes_per_sector * fs->_boot.sectors_per_cluster;


#ifdef MEM_DEBUG
    printf( "Allocating %d clusters for %d\n", size, uid );
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
