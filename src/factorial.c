#include <factorial.h>

unsigned int factorial( unsigned short n )	
{
    if ( n <= 1 ) return 1;
    return n * factorial( n - 1 );
}

