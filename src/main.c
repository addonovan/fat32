#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <filesystem.h>
#include <command.h>

#define MAX_LINE_LENGTH     2048
#define MAX_ARG_LENGTH      512
#define MAX_ARGS            MAX_LINE_LENGTH / ( MAX_ARG_LENGTH )

/**
 * Reads a single line of input from `stdin` into `line`, then splits
 * it into tokens by whitespace and writes pointers to each in `argv`.
 * 
 * Returns the number of arguments filled into `argv`.
 */
int read_line( char* line, char** argv );

int main()
{
    filesystem_t* fs = NULL;

    // parameters we read in from the command line
    char* line = malloc( MAX_LINE_LENGTH );
    char** argv = malloc( sizeof( char* ) * MAX_ARGS );

    for ( int i = 0; i < MAX_ARGS; i++ )
    {
        argv[ i ] = NULL;
    }

    while ( true )
    {
        printf( "mfs> " );

        // read a single line
        int argc = read_line( line, argv ) - 1;

        if ( !try_commands( &fs, argc, argv ) )
        {
            printf( "Unrecognized command: %s\n", argv[ 0 ] );
        }
    }

    return 0;
}

int read_line( char* line, char** argv )
{
    fgets( line, MAX_LINE_LENGTH, stdin );

    // zero out all of the previous argv
    memset( argv, 0, MAX_ARGS * sizeof( char* ) );

    // tokenize the string
#define WHITESPACE " \r\n\t"
    int i = 0;
    argv[ i ] = strtok( line, WHITESPACE );
    while ( argv[ i ] != NULL )
    {
        i++;
        argv[ i ] = strtok( NULL, WHITESPACE );
    }
#undef WHITESPACE

    return i;
}
