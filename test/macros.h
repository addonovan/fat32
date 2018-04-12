#ifndef __TEST_MACROS_H__
#define __TEST_MACROS_H__

#include <string.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

#ifndef MAX_DESC_LENGTH
#define MAX_DESC_LENGTH 1024
#endif

#define var( name, value ) __typeof__( value ) name = value

#define test( description, block )\
    void handler( int sig );\
    void* create_shared_memory( size_t size );\
    char* test_desc = NULL;\
    char* case_desc = NULL;\
    unsigned int* asserts = NULL;\
    unsigned int* assert_fails = NULL;\
    unsigned int* tests = NULL;\
    unsigned int* test_fails = NULL;\
    int main()\
    {\
        signal( SIGSEGV, handler );\
        test_desc = calloc( sizeof( char ), MAX_DESC_LENGTH );\
        case_desc = calloc( sizeof( char ), MAX_DESC_LENGTH );\
        asserts = create_shared_memory( sizeof( unsigned int ) );\
        assert_fails = create_shared_memory( sizeof( unsigned int ) );\
        tests = create_shared_memory( sizeof( unsigned int ) );\
        test_fails = create_shared_memory( sizeof( unsigned int ) );\
        strcpy( test_desc, description );\
        printf( "%s\n", test_desc );\
        { block }\
        free( case_desc );\
        free( test_desc );\
        printf( "=======================================\n" );\
        printf(\
            "  %3d test case%s  ||  %3d failure%s\n",\
            *tests, ( *tests == 1 ? "" : "s" ),\
            *test_fails, ( *test_fails == 1 ? "" : "s" )\
        );\
        printf(\
            "  %3d assertion%s  ||  %3d failure%s\n",\
            *asserts, ( *asserts == 1 ? "" : "s" ),\
            *assert_fails, ( *assert_fails == 1 ? "" : "s" )\
        );\
        printf( "=======================================\n" );\
        printf( "\n\n" );\
    }\
    void handler( int sig )\
    {\
        void *array[ 10 ];\
        size_t size;\
        size = backtrace( array, 10 );\
        fprintf( stderr, "Error: signal %d:\n", sig );\
        backtrace_symbols_fd( array, size, STDERR_FILENO );\
        exit( sig );\
    }\
    void* create_shared_memory( size_t size )\
    {\
        int protection = PROT_READ | PROT_WRITE;\
        int visibility = MAP_ANONYMOUS | MAP_SHARED;\
        return mmap( NULL, size, protection, visibility, 0, 0 );\
    }\

#define on( description, block )\
    {\
        char desc[] = description;\
        memcpy( case_desc, desc, strlen( desc ) );\
        *tests += 1;\
        printf( "\t%s\n", case_desc );\
        pid_t child = fork();\
        if ( child < 0 )\
        {\
            perror( "fork()" );\
        }\
        else if ( child == 0 )\
        {\
            { block }\
            exit( 0 );\
        }\
        else\
        {\
            int status;\
            waitpid( child, &status, 0 );\
            fflush( stdout );\
            fflush( stderr );\
            if ( ( WIFEXITED( status ) && WEXITSTATUS( status ) != 0 ) || WIFSIGNALED( status ) )\
            {\
                *test_fails += 1;\
                printf( "\n" );\
            }\
        }\
        memset( case_desc, 0, MAX_DESC_LENGTH );\
    }

#define var( name, value ) __typeof__( value ) name = value

#define require( formatter, expected, actual )\
    {\
        *asserts += 1;\
        var( ex_actual, actual );\
        var( ex_expected, expected );\
        if ( ex_actual != ex_expected )\
        {\
            printf( "\tFAILED %s\n", __FILE__ );\
            printf( "\t\tassert %s == %s\n", #actual, #expected );\
            printf( "\t\t\tExpected: " formatter "\n", ex_expected );\
            printf( "\t\t\tActual:   " formatter "\n", ex_actual );\
            *assert_fails += 1;\
            exit( 1 );\
        }\
    }

#define str_require( expected, actual )\
    {\
        *asserts += 1;\
        char* ex_actual = actual;\
        char* ex_expected = expected;\
        if ( strcmp( ex_actual, ex_expected ) != 0 )\
        {\
            printf( "\tFAILED %s\n", __FILE__ );\
            printf( "\t\tassert " #actual " == " #expected "\n" );\
            printf( "\t\t\tExpected: \"%s\"\n", ex_expected );\
            printf( "\t\t\tActual:   \"%s\"\n", ex_actual );\
            *assert_fails += 1;\
            exit( 1 );\
        }\
    }

#define require_nonnull( actual )\
    {\
        *asserts += 1;\
        var( ex_actual, actual );\
        if ( ex_actual == NULL )\
        {\
            printf( "\tFAILED %s\n", __FILE__ );\
            printf( "\t\tassert " #actual " != NULL\n" );\
            *assert_fails += 1;\
            exit( 1 );\
        }\
    }

#endif