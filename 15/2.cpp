#include <iostream>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <iterator>


size_t constexpr target{ 30000000 };


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::unordered_map< size_t, size_t > numbers;

	size_t n( 0 ), x( 0 ), t( 0 );

	while( !ifs.eof() )
	{
		ifs >> x;
		ifs.ignore( 1 );

		if( !ifs.eof() )
			numbers[ x ] = ++n;
	}

	while( n != target - 1 )
	{
		std::unordered_map< size_t, size_t >::const_iterator i{ numbers.find( x ) };
		if( i != numbers.end() )
			t = n - i->second + 1;
		else
			t = 0;

		numbers[ x ] = ++n;
		x = t;
	}

	std::cout << target << "th is " << t << std::endl;

	return 0;
}