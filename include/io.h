#ifndef __IO_H__
#define __IO_H__

#include <stdio.h>
#include <stdlib.h>
#include <filesystem.h>

//
// Offset/Sector/Cluster conversion
//

/**
 * Converts a cluster id to an offset into a file.
 */
offset_t cluster_to_offset( const filesystem_t* fs, cluster_t cluster );

/**
 * Converts a sector id to an offset into a file.
 */
offset_t sector_to_offset( const filesystem_t* fs, sector_t sector );

//
// Filesystem reading
//

/**
 * Reads a section of the filesystem. Offset is the offset from the start
 * of the file system (i.e. 0 is the beginning of the boot sectors).
 * 
 * Returns if the read was successful or not.
 */
bool io_read( const filesystem_t* fs, offset_t start, offset_t length, void* dst );

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

/**
 * Gets the successor cluster to the current cluster. This will look
 * in the FAT and will write the following cluster id to `cluster` if
 * one exists.
 *
 * Returns if the cluster has a successor or not. If the cluster does
 * not have a successor, then `cluster` will not be updated.
 */
bool io_next_cluster( const filesystem_t* fs, cluster_t* cluster );

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
 * Allocates `size` clusters of memory on the heap with an
 * additional trailing byte for a null-terminator.
 */
void* io_clalloc( const filesystem_t* fs, unsigned int size );

/**
 * Free's a block of memory allocated by `io_alloc`
 */
void io_free( void* block );

#endif
