#include <input.h>
#include <string.h>

void input_read( input_t* this, FILE* stream )
{
    // make sure that we have null terminators everywhere
    // before reading the source
    memset( this, '\0', sizeof( *this ) );

    fgets( this->input, sizeof( this->input ), stream );
}

int input_matches( input_t* this, const char* format, ... )
{
    va_list args;
    va_start( args, format );
    int matches = vsscanf( this->input, format, args );
    va_end( args );

    return matches;
}
