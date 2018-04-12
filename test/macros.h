/**
 * This file is absolutely horrid.
 *
 * This may be the worse use of macros I've ever used, but I'm
 * proud of how it turned out in the end (the syntax for the actual
 * tests reads pretty nicely, in my opinion).
 *
 * Each test is run in its own subprocess, that way if the process
 * dies due to a segfault or something, then the other test cases
 * can still run.
 *
 * There is a memory map shared between all of the processes which
 * allows them to sync details, such as the number of test cases
 * run (and those which failed), as well as the number of assertions
 * and assertion failures.
 *
 * Should a segfault happen, a signal handler has been installed
 * in all of the processes which will unwind the stack and print
 * a trace of it; however, this is in file offsets and not line
 * numbers, but it is better than nothing.
 *
 * If, for some reason, a test description or case description is
 * longer than `1023` characters, then you can define `MAX_DESC_LENGTH`
 * to a larger number before including this file.
 */

#ifndef __TEST_MACROS_H__
#define __TEST_MACROS_H__

#include <string.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#ifndef MAX_DESC_LENGTH
#define MAX_DESC_LENGTH 1024
#endif

/**
 * Describes a test of the system described by `description`.
 */
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
        printf( "Testing %s\n", test_desc );\
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
        return 0;\
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

/**
 * A `block` of code to test, described in plain language by `description`, which
 * will run *in a separate process* than the test process.
 */
#define on( description, block )\
    {\
        char desc[] = description;\
        memcpy( case_desc, desc, strlen( desc ) );\
        *tests += 1;\
        printf( "\tOn %s\n", case_desc );\
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

/**
 * Uses a GNU extension to emulate the `auto` keyword of C++11
 */
#define var( name, value ) __typeof__( value ) name = value

/**
 * Requires the value-type `actual` to be *exactly* equal to the `expected`
 * value. Should this case fail `formatter` will be used to print the values
 * of both the expected and actual.
 */
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

/**
 * Requires the string, `actual`, to be equal to the `expected`
 * string. Strings are compared with `strcmp`.
 */
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

/**
 * Checks if `actual` is not equal to `NULL`. This will fail only if
 * `actual == NULL`.
 */
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
