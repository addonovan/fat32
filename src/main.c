#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <filesystem.h>

#define MAX_LINE_LENGTH     2048
#define MAX_ARG_LENGTH      512
#define MAX_ARGS            MAX_LINE_LENGTH / ( MAX_ARG_LENGTH )

int read_line( char* line, char** args );

int main()
{
    bool initialized = false;
    filesystem_t fs;

    // parameters we read in from the command line
    char* line      = malloc( MAX_LINE_LENGTH );
    char** args = malloc( sizeof( char* ) * MAX_ARGS );
    for ( int i = 0; i < MAX_ARGS; i++ )
    {
        args[ i ] = NULL;
    }

    while ( true )
    {
        printf( "mfs> " );

        // read a single line
        int argc = read_line( line, args ) - 1;

#define COMMAND( s, c, b ) \
        if ( strcmp( args[ 0 ], s ) == 0 )\
        {\
            if ( argc >= c ) b\
            else printf( "Wrong number of arguments (found %d, expected %d)\n", argc, c );\
            continue;\
        }\

        COMMAND( "quit", 0, { break; } );
        COMMAND( "exit", 0, { break; } );

        if ( !initialized )
        {
            COMMAND( "open", 1, {
                filesystem_init( &fs, args[ 1 ] );
                initialized = true;
            } );

            printf( "Filesystem must be mounted first\n" );
            continue;
        }

        COMMAND( "open", 1, {
            printf( "Filesystem is already mounted.\n" );
            printf( "Close the current filesystem first\n" );
        } );

        COMMAND( "close", 0, {
            // filesystem_close( &fs );
            printf( "close not yet implemented!\n" );
            initialized = false;
        } );

        COMMAND( "info", 0, {
            // filesystem_info( &fs );
            printf( "info not yet implemented!\n" );
        } );

        COMMAND( "stat", 1, {
            // filesystem_stat( &fs, args[ 1 ] );
            printf( "stat is not yet implemented!\n" );
        } );

        COMMAND( "get", 1, {
            // FILE* file = fopen( args[ 1 ], "w+" );
            // filesystem_get( &fs, file );
            // fclose( file );
            printf( "get not yet implemented!\n" );
        } );

        COMMAND( "cd", 1, {
            if ( !filesystem_cd( &fs, args[ 1 ] ) )
            {
                printf( "Error: File not found\n" );
            }
        } );

        COMMAND( "ls", 0, {
            directory_t dir = filesystem_list( &fs );

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

            free( dir.files );
        } );

        COMMAND( "read", 3, {
            // char* file_name = args[ 1 ];
            // offset_t position = atoi( args[ 2 ] );
            // offset_t length = atoi( args[ 3 ] );
            // filesystem_read( &fs, file_name, position, length );
            printf( "read not yet implemented!\n" );
        } );

        COMMAND( "volume", 0, {
            char* file_name = fs._boot.volume_label;
            if ( strlen( file_name ) == 0 )
            {
                printf( "Error: volume name not found." );
            }
            else
            {
                printf( "%s", file_name );
            }
        } );

#undef COMMAND

        printf( "Unrecognized command: %s\n", args[ 0 ] );

    }

    return 0;
}

int read_line( char* line, char** args )
{
    fgets( line, MAX_LINE_LENGTH, stdin );

    // zero out all of the previous args
    memset( args, 0, MAX_ARGS * sizeof( char* ) );

    // tokenize the string
#define WHITESPACE " \r\n\t"
    int i = 0;
    args[ i ] = strtok( line, WHITESPACE );
    while ( args[ i ] != NULL )
    {
        i++;
        args[ i ] = strtok( NULL, WHITESPACE );
    }
#undef WHITESPACE

    return i;
}
