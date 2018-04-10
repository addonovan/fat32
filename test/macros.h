#ifndef __TEST_MACROS_H__
#define __TEST_MACROS_H__

#include <string.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define var( name, value ) __typeof__( value ) name = value

#define test_init()\
    {\
        signal( SIGSEGV, handler );\
    }\

#define require( formatter, expected, actual )\
    {\
        var( ex_actual, actual );\
        var( ex_expected, expected );\
        if ( ex_actual != ex_expected )\
        {\
            printf( "FAILED %s:%d\n", __FILE__, __LINE__ );\
            printf( "\tassert %s == %s\n", #actual, #expected );\
            printf( "\t\tExpected: " formatter "\n", ex_expected );\
            printf( "\t\tActual:   " formatter "\n", ex_actual );\
            exit( 1 );\
        }\
    }

#define str_require( expected, actual )\
    {\
        char* ex_actual = actual;\
        char* ex_expected = expected;\
        if ( strcmp( ex_actual, ex_expected ) != 0 )\
        {\
            printf( "FAILED %s:%d\n", __FILE__, __LINE__ );\
            printf( "\tassert " #actual " == " #expected "\n" );\
            printf( "\t\tExpected: \"%s\"\n", ex_expected );\
            printf( "\t\tActual:   \"%s\"\n", ex_actual );\
            exit( 1 );\
        }\
    }

#define require_nonnull( actual )\
    {\
        var( ex_actual, actual );\
        if ( ex_actual == NULL )\
        {\
            printf( "FAILED %s:%d\n", __FILE__, __LINE__ );\
            printf( "\tassert " #actual " != NULL\n" );\
            exit( 1 );\
        }\
    }

void handler( int sig )
{
  void *array[ 10 ];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace( array, 10 );

  // print out all the frames to stderr
  fprintf( stderr, "Error: signal %d:\n", sig );
  backtrace_symbols_fd( array, size, STDERR_FILENO );
  exit( 1 );
}

#endif