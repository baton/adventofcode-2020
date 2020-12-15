#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>


size_t constexpr target{ 2020 };


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< size_t > numbers;
	numbers.reserve( target );

	while( !ifs.eof() )
	{
		size_t x( 0 );
		ifs >> x;

		numbers.push_back( x );

		ifs.ignore( 1 );
	}

	while( numbers.size() != target )
	{
		size_t x( 0 );

		std::vector< size_t >::const_reverse_iterator i{
			std::find( std::next( numbers.crbegin() ), numbers.crend(), numbers.back() ) };

		if( i != numbers.crend() )
			x = std::distance( std::next( numbers.crbegin() ), i ) + 1;

		numbers.push_back( x );
	}

	std::cout << numbers.size() << "th is " << numbers.back() << std::endl;

	return 0;
}