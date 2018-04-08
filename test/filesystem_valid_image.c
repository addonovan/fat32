#include <file.h>
#include <filesystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "macros.h"

int main()
{
    filesystem_t fs;
    require( "%d", true, filesystem_init( &fs, "res/fat32.img" ) );

    require( "%u", 16384u,  fs.fat );
    require( "%u", 1049600u, fs.clusters );

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

    // test fetching files

    get(&fs, "BAR");
    FILE* bar = fopen("BAR", "r");
    char text[9];
    fread(text, 8, 1, bar);
    text[8] = '\0';
    str_require("5 6 7 8\n", text);
    fclose(bar);

    // test changing directories
    require( "%d", true, filesystem_cd( &fs, "FOLDERA" ) );
    require( "%u", 6099u, fs.cwd );

    // ensure that filesystem_list works in more than the root dir
    dir = filesystem_list( &fs );
    require( "%u", 3u, dir.count );
    str_require( ".",           dir.files[ 0 ].name );
    str_require( "..",          dir.files[ 1 ].name );
    str_require( "FOLDERC",     dir.files[ 2 ].name );
    free( dir.files );

    // test the . special file
    require( "%d", true, filesystem_cd( &fs, "." ) );
    require( "%u", 6099u, fs.cwd );
    dir = filesystem_list( &fs );
    require( "%u", 3u, dir.count );
    free( dir.files );
    
    // test the .. special file
    require( "%d", true, filesystem_cd( &fs, ".." ) );
    require( "%u", 2u, fs.cwd );
    dir = filesystem_list( &fs );
    require( "%u", 7u, dir.count );
    free( dir.files );

    return 0;
}
