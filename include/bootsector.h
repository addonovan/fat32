#ifndef __BOOTSECTOR_H__

#define __BOOTSECTOR_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * A struct representing all of the (relevant) parameters found 
 * inside the bootsector of the drive, which includes the BPB.
 */
typedef struct 
{
    // these don't need to be as small as they are on the
    // disk, since I won't actually write any of it back

    char        oem_name[ 9 ];          // BS_OEMName (1 additional byte for \0)
    int32_t     bytes_per_sector;       // BPB_BytsPerSec
    int32_t     sectors_per_cluster;    // BPB_SecPerClus
    int32_t     reserved_sector_count;  // BPB_RsvdSecCnt
    int32_t     fat_count;              // BPB_NumFATs
    int32_t     root_entry_count;       // BPB_RootEntCnt
    int32_t     fat_sector_count;       // BPB_FATSz32
    int32_t     root_cluster;           // BPB_RootClus
    char        volume_label[ 11 ];     // BS_VolLab (1 additional byte for \0)

} bootsector_t;

/**
 * Initializes a bootsector structure by reading the given FAT32
 * image.
 */
bool bootsector_init( bootsector_t*, FILE* file );

#endif
