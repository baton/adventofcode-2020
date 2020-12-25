#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include <numeric>


uint64_t constexpr divider = 20201227;

uint64_t transform_key( uint64_t const subject, uint64_t const key )
{
	uint64_t r( 1 );
	for( uint64_t i( 0 ); i != key; ++i )
	{
		r *= subject;
		r %= divider;
	}
	return r;
}

uint64_t loop_size( uint64_t const subject, uint64_t const key )
{
	uint64_t r( 1 );
	uint64_t loops( 0 );

	while( r != key )
	{
		r *= subject;
		r %= divider;
		++loops;
	}

	return loops;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	uint64_t card_key( 0 ), door_key( 0 );
	ifs >> card_key >> door_key;

	uint64_t card_loop_size( loop_size( 7, card_key ) );
	uint64_t card_e_key( transform_key( door_key, card_loop_size ) );

	std::cout << "key = " << card_e_key << std::endl;

	return 0;
}