#include <filesystem.h>
#include <io.h>
#include "macros.h"

test( "Fundamental IO api", {

    filesystem_t fs;
    require( "%d", true, filesystem_init( &fs, "res/fat32.img" ) );

#ifdef MEM_DEBUG
    on( "tagged sector allocation", {
        unsigned int* block = io_alloc( &fs, 1 );
        require( "%u", 0u, block[ -1 ] );
        io_free( block );

        block = io_alloc( &fs, 1 );
        require( "%u", 1u, block[ -1 ] );
        io_free( block );
    } );

    on( "tagged cluster allocation", {
        unsigned int* cluster = io_clalloc( &fs, 1 );
        require( "%u", 0u, cluster[ -1 ] );
        io_free( cluster );

        cluster = io_clalloc( &fs, 1 );
        require( "%u", 1u, cluster[ -1 ] );
        io_free( cluster );
    } );

    on( "mixed sector and cluster allocations", {
        unsigned int* block = io_alloc( &fs, 1 );
        require( "%u", 0u, block[ -1 ] );
        
        unsigned int* cluster = io_clalloc( &fs, 1 );
        require( "%u", 1u, cluster[ -1 ] );

        io_free( block );
        io_free( cluster );
    } );
#endif

    on( "io unit conversions", {

        on( "sector to offset conversion", {
            require( "%d", fs.fat, sector_to_offset( &fs, 0u ) );
            // TODO maybe add more test cases?
        } );

        on( "cluster to offset conversion", {
            require( "%d", fs.clusters, cluster_to_offset( &fs, fs._boot.root_cluster ) );
            // TODO maybe add more test cases?
        } );

    } );

    on( "reading a cluster", {
        char* cluster = ( char* ) io_clalloc( &fs, 1 );
        require( "%d", true, io_read_cluster( &fs, 17, cluster ) );
        str_require( "5 6 7 8\n", cluster );
        io_free( cluster );
    } );

    filesystem_close( &fs );
} );
