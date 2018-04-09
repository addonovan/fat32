#include <command.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//
// Macros for command DSL
//

#define UNIMPLEMENTED()\
    printf( "%s: not yet implemented\n", argv[ 0 ] );\
    return true;

#define COMMAND_FUNC( name ) sys_##name

#define COMMAND( name, block )\
    bool COMMAND_FUNC( name ) ( filesystem_t** fs, int argc, char** argv )\
    {\
        int i = 0;\
        ( void )( i );\
        if ( strcmp( argv[ 0 ], #name ) != 0 ) return false;\
        \
        ( void )( argc );\
        block\
        return true;\
    }

#define TRY_COMMAND( command )\
    if ( COMMAND_FUNC( command )( fs, argc, argv ) ) return true;

#define REQUIRE( text, cond )\
    if ( !( cond ) )\
    {\
        printf( "%s: %s\n", argv[ 0 ], text );\
        return true;\
    }

#define REQUIRE_FS() REQUIRE( "No filesystem mounted", *fs != NULL )


#define STRING( data ) data
#define INTEGER( data ) atoi( data )

#define ARG( name, parser )\
    if ( i == argc )\
    {\
        printf( "Missing argument: %s\n", #name );\
        return true;\
    }\
    else\
    {\
        i++;\
        name = parser( *( argv + i ) );\
    }

//
// Command Definitions
//

COMMAND( open, {
    REQUIRE( "Filesystem already mounted", *fs == NULL )

    char* file_name;
    ARG( file_name, STRING );

    filesystem_t* new_fs = malloc( sizeof( **fs ) );
    if ( filesystem_init( new_fs, file_name ) )
    {
        printf( "Mounted %s\n", file_name );
        *fs = new_fs;
    }
    else
    {
        free( *fs );
    }
} );

COMMAND( close, {
    REQUIRE_FS();

    // filesystem_close( fs );
    free( fs );
    fs = NULL;
    UNIMPLEMENTED();
} );

COMMAND( info, {
    REQUIRE_FS();
    UNIMPLEMENTED();
} );

COMMAND( stat, {
    REQUIRE_FS();
    UNIMPLEMENTED();
} );

COMMAND( get, {
    REQUIRE_FS();
    UNIMPLEMENTED();
} );

COMMAND( cd, {
    REQUIRE_FS();

    char* file_name;
    ARG( file_name, STRING );

    if ( !filesystem_cd( *fs, file_name ) )
    {
        printf( "No such directory: %s\n", file_name );
        printf( "Are you sure it's a directory?\n" );
    }
} );

COMMAND( ls, {
    REQUIRE_FS();

    directory_t dir = filesystem_list( *fs );
    for ( unsigned int i = 0u; i < dir.count; i++ )
    {
        file_t file = dir.files[ i ];
        if ( file.attrs.directory )
        {
            printf( "%s\n", file.name );
        }
        else
        {
            printf( "%s.%s\n", file.name, file.ext );
        }
    }
} );

COMMAND( read, {
    REQUIRE_FS();

    // char* file_name;
    // offset_t start;
    // offset_t length;

    // ARG( file_name, STRING );
    // ARG( start, INTEGER );
    // ARG( length, INTEGER );

    UNIMPLEMENTED();    
} );

COMMAND( volume, {
    REQUIRE_FS();

    char* file_name = ( *fs )->_boot.volume_label;

    if ( strlen( file_name ) == 0 )
    {
        printf( "Error: volume name not found.\n" );
    }
    else
    {
        printf( "%s\n", file_name );
    }
} );

COMMAND( quit, {
    REQUIRE( "Please dismount the filesystem first", *fs == NULL );
    exit( 0 );
} );

COMMAND( exit, {
    TRY_COMMAND( quit );
} );

//
// Command Runner
//

bool try_commands( filesystem_t** fs, int argc, char** argv )
{
    TRY_COMMAND( quit );
    TRY_COMMAND( exit );
    TRY_COMMAND( open );
    TRY_COMMAND( close );
    TRY_COMMAND( info );
    TRY_COMMAND( stat );
    TRY_COMMAND( get );
    TRY_COMMAND( cd );
    TRY_COMMAND( cd );
    TRY_COMMAND( ls );
    TRY_COMMAND( read );
    TRY_COMMAND( volume );

    // couldn't find a command to run
    return false;
}
