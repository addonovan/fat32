#include <stdio.h>
#include <input.h>
#include "macros.h"

int main()
{
    FILE* input_matching = fopen( "res/input_matching.txt", "r" );

    input_t input;
    input_read( &input, input_matching );

    int a, b, c, d;
    require( "%d", 4, input_matches( &input, "test %d %d %d %d\n", &a, &b, &c, &d ) );
    require( "%d", 1, a );
    require( "%d", 2, b );
    require( "%d", 3, c );
    require( "%d", 4, d );

    char e[ INPUT_MAX_LINE_LENGTH ];
    require( "%d", 4, input_matches( &input, "test %d %d %d %s", &a, &b, &c, &e ) );
    require( "%d", 1, a );
    require( "%d", 2, b );
    require( "%d", 3, c );
    require( "%d", 4, d );
    str_require( "4", e );

    memset( e, '\0', sizeof( e ) );
    require( "%d", 4, input_matches( &input, "test %d %d %d %d %s", &a, &b, &c, &d, &e ) );
    require( "%d", 1, a );
    require( "%d", 2, b );
    require( "%d", 3, c );
    require( "%d", 4, d );
    str_require( "", e );

    // check the rest of the file
    char file_name[ INPUT_MAX_LINE_LENGTH ];
    unsigned int start;
    unsigned int length;

    input_read( &input, input_matching );
    require( "%d", 1, input_matches( &input, "open %s", file_name ) );
    str_require( "res/fat32.img", file_name );

    input_read( &input, input_matching );
    require( "%d", 0, input_matches( &input, "close" ) );

    input_read( &input, input_matching );
    require( "%d", 1, input_matches( &input, "stat %s", file_name ) );
    str_require( "FOLDERA", file_name );

    input_read( &input, input_matching );
    require( "%d", 1, input_matches( &input, "stat %s", file_name ) );
    str_require( "num.txt", file_name );

    input_read( &input, input_matching );
    require( "%d", 3, input_matches( &input, "read %s %d %d", file_name, &start, &length ) );
    str_require( "num.txt", file_name );
    require( "%d", 0u, start );
    require( "%d", 100u, length );

    fclose( input_matching );

    return 0;
}
