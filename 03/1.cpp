#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>


struct line_t : std::string
{};

std::istream & operator>>( std::istream & is, line_t & line )
{
	return std::getline( is, line );
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< std::string > const lines{ std::istream_iterator< line_t >( ifs ), std::istream_iterator< line_t >() };

	size_t x( 0 ), count( 0 );
	for( size_t i( 0 ); i != lines.size(); ++i )
	{
		if( lines[ i ][ x % lines[ i ].length() ] == '#' )
			++count;
		x += 3;
	}

	std::cout << "count = " << count << std::endl;

	return 0;
}