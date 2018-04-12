#include <bootsector.h>
#include <stdio.h>
#include "macros.h"

int main()
{
    test_init();

    FILE* file = fopen( "src/bootsector.c", "r" );
    require_nonnull( file );

    bootsector_t boot;
    require( "%d", false, bootsector_init( &boot, file ) );

    return 0;
}
