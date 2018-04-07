#ifndef __IO_H__
#define __IO_H__

#include <stdio.h>
#include <stdlib.h>
#include <filesystem.h>

typedef uint32_t    sector_t;
typedef uint32_t    cluster_t;

//
// Filesystem reading
//

/**
 * Reads a sector from the filesystem directly into [dst]. 
 * This sector is relative to the start of the first FAT.
 *
 * Returns if the read was successful or not.
 */
bool io_read_sector( const filesystem_t* fs, sector_t sector, void* dst );

/**
 * Reads a cluster form the filesystem directly into [dst].
 * This cluster is the cluster number in the data section of
 * the image.
 *
 * Returns if the read was successful or not.
 */
bool io_read_cluster( const filesystem_t* fs, cluster_t cluster, void* dst );

//
// Memory Allocation
//

/**
 * Allocates `size` sectors of memory on the heap with an 
 * additional trailing byte for a null-terminator. (for
 * a total size of `BPB_BytsPerSec * size + 1` bytes).
 */
void* io_alloc( const filesystem_t* fs, unsigned int size );

/**
 * Free's a block of memory allocated by `io_alloc`
 */
void io_free( void* block );

#endif
