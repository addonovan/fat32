#include <include/catch.hpp>

unsigned int factorial( unsigned int input )
{
  if ( input < 2 ) return 1;
  return input * factorial( input - 1 );
}

TEST_CASE( "factorials", "[factorial]" )
{
  REQUIRE( factorial( 0 ) == 1 );
  REQUIRE( factorial( 1 ) == 1 );
  REQUIRE( factorial( 2 ) == 2 );
  REQUIRE( factorial( 3 ) == 6 );
}


