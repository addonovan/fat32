#include <file.h>
#include <filesystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "macros.h"

test( "fileystem with a valid image", {
    filesystem_t fs;
    require( "%d", true, filesystem_init( &fs, "res/fat32.img" ) );

    on( "browsing root directory", {
        require( "%u", 16384u,  fs.fat );
        require( "%u", 1049600u, fs.clusters );
        str_require( "/", fs.dir );

        // test the contents of the root directory
        directory_t dir = filesystem_list( &fs );

        require( "%u", 7u, dir.count );
        str_require( "BAR",         dir.files[ 0 ].name );
        str_require( "NUM",         dir.files[ 1 ].name );
        str_require( "DEADBEEF",    dir.files[ 2 ].name );
        str_require( "FOO",         dir.files[ 3 ].name );
        str_require( "FOLDERA",     dir.files[ 4 ].name );
        str_require( "FATSPEC",     dir.files[ 5 ].name );
        str_require( "FOLDERB",     dir.files[ 6 ].name );
        free( dir.files );
    } );

    on( "getting bar.txt", {
        filesystem_get( &fs, "bar.txt" );
        FILE* bar = fopen( "bar.txt", "r" );
        require_nonnull( bar );

        char text[ 9 ];
        fread( text, 8, 1, bar );
        text[ 8 ] = '\0';
        str_require( "5 6 7 8\n", text );

        require( "%d", 0, fclose( bar ) );
        require( "%d", 0, remove( "bar.txt" ) );
    } );

    on( "changing directories", {
        str_require( "/", fs.dir );
        require( "%d", true, filesystem_cd( &fs, "FOLDERA" ) );
        require( "%u", 6099u, fs.cwd );
        str_require( "/FOLDERA/", fs.dir );

        // ensure that filesystem_list works in more than the root dir
        directory_t dir = filesystem_list( &fs );
        require( "%u", 3u, dir.count );
        str_require( ".",           dir.files[ 0 ].name );
        str_require( "..",          dir.files[ 1 ].name );
        str_require( "FOLDERC",     dir.files[ 2 ].name );
        free( dir.files );
    } );

    on( "navigating to .", {
        require( "%d", true, filesystem_cd( &fs, "foldera" ) );

        require( "%d", true, filesystem_cd( &fs, "." ) );
        require( "%u", 6099u, fs.cwd );
        str_require( "/FOLDERA/", fs.dir );
        directory_t dir = filesystem_list( &fs );
        require( "%u", 3u, dir.count );
        free( dir.files );
    } );

    on( "navigating to ..", {
        require( "%d", true, filesystem_cd( &fs, "foldERa" ) );

        require( "%d", true, filesystem_cd( &fs, ".." ) );
        require( "%u", 2u, fs.cwd );
        str_require( "/", fs.dir );
        directory_t dir = filesystem_list( &fs );
        require( "%u", 7u, dir.count );
        free( dir.files );
    } );

    on( "stat-ing an existing file", {
        require( "%d", true, filesystem_stat( &fs, "foo.txt" ) );
    } );

    on( "stat-ing a non-existing file", {
        require( "%d", false, filesystem_stat( &fs, "txt.foo" ) );
    } );

    // test file closing
    filesystem_close( &fs );
} );
