#ifndef __BOOTSECTOR_H__

#define __BOOTSECTOR_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct bootsector_t bootsector_t;

struct bootsector_t
{
    char        oem_name[ 11 ];          // BS_OEMName (1 additional byte for \0)
    int16_t     bytes_per_sector;       // BPB_BytsPerSec
    int8_t      sectors_per_cluster;    // BPB_SecPerClus
    int16_t     reserved_sector_count;  // BPB_RsvdSecCnt
    int8_t      fat_count;              // BPB_NumFATs
    int16_t     root_entry_count;       // BPB_RootEntCnt
    int32_t     fat_sector_count;       // BPB_FATSz32
    int32_t     root_cluster;           // BPB_RootClus
    char        volume_label[ 13 ];     // BS_VolLab (1 additional byte for \0)

};

/**
 * Initializes a bootsector structure by reading the given FAT32
 * image.
 */
bool bootsector_init( bootsector_t*, FILE* file );

#endif
