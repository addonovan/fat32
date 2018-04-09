#include <command.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//
// Macros for command DSL
//

/** Converts the command's name into a special name to prevent duplicates */
#define COMMAND_FUNC( name ) command_##name

/**
 * Defines a new method that will look like this:
 *
 * ```c
 * void sys_${name}( filesystem_t** fs, int argc, char** argv )
 * {
 *   int i = 0;
 *   ${block}()
 * }
 * ```
 */
#define COMMAND( name, block )\
    void COMMAND_FUNC( name ) ( filesystem_t** fs, int argc, char** argv )\
    {\
        int i = 0;\
        ( void )( i );\
        ( void )( argc );\
        block\
    }

/** 
 * Tries to run the command with the given name.
 *
 * If the name matches the command's name, then it will try to
 * run the command, and afterwars will return true to denote that
 * a command has been found.
 */
#define TRY_COMMAND( command )\
    if ( strcmp( argv[ 0 ], #command ) == 0 )\
    {\
        COMMAND_FUNC( command )( fs, argc, argv + 1 );\
        return true;\
    }\

/** Defines a command with the name `alias` that simply calls `command` */
#define ALIAS( command, alias )\
    COMMAND( alias, {\
        COMMAND_FUNC( command )( fs, argc, argv );\
    } );\


/** Prints a message that the command has not yet been implemented */
#define UNIMPLEMENTED()\
    printf( "%s: not yet implemented\n", argv[ -1 ] );\
    return;

/** Requires that `cond` be true. If it isn't then `text` is printed out. */
#define REQUIRE( text, cond )\
    if ( !( cond ) )\
    {\
        printf( "%s: %s\n", argv[ -1 ], text );\
        return;\
    }

/** Requires that there be a mounted filesystem to run a command. */
#define REQUIRE_FS() REQUIRE( "No filesystem mounted", *fs != NULL )

/** Parses an argument as a char*. See ARG */
#define STRING( data ) data

/** Parses an argument as an integer. See ARG */
#define INTEGER( data ) atoi( data )

/** 
 * Tries to parse the next argument with `parser` and will save
 * the value to the variable named `name`.
 */
#define ARG( name, parser )\
    if ( i == argc )\
    {\
        printf( "Missing argument: %s\n", #name );\
        return;\
    }\
    else\
    {\
        name = parser( *( argv + i ) );\
        i++;\
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
ALIAS( open, mount );

COMMAND( close, {
    REQUIRE_FS();

    // filesystem_close( fs );
    free( fs );
    fs = NULL;
    UNIMPLEMENTED();
} );
ALIAS( close, dismount );
ALIAS( close, umount );
ALIAS( close, unmount );

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

#define DELIMITTER "/\\"
COMMAND( cd, {
    REQUIRE_FS();

    char* file_name;
    ARG( file_name, STRING );

    cluster_t cwd = ( **fs ).cwd;

    // continually cd on every path spec
    char* token = strtok( file_name, DELIMITTER );

    // if it's blank, then we're an absolute path from the root
    // so, let's navigate back up to the root directory again
    while ( filesystem_cd( *fs, ".." ) );

    while ( token != NULL && filesystem_cd( *fs, token ) )
    {
        token = strtok( NULL, DELIMITTER );
    }

    if ( token != NULL )
    {
        // revert to previous directory
        ( **fs ).cwd = cwd;
        printf( "No such directory: %s\n", file_name );
        printf( "Are you sure it's a directory?\n" );
    }
} );
#undef DELIMITTER

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
ALIAS( quit, exit );

//
// Command Runner
//

bool try_commands( filesystem_t** fs, int argc, char** argv )
{
    TRY_COMMAND( quit );
    TRY_COMMAND( exit );

    TRY_COMMAND( open );
    TRY_COMMAND( mount );

    TRY_COMMAND( close );
    TRY_COMMAND( dismount );
    TRY_COMMAND( umount );
    TRY_COMMAND( unmount );

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
