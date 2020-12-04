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


size_t count_trees( std::vector< std::string > const & lines, size_t const delta_x, size_t const delta_y )
{
	size_t x( 0 ), count( 0 );
	for( size_t i( 0 ); i < lines.size(); i += delta_y )
	{
		if( lines[ i ][ x % lines[ i ].length() ] == '#' )
			++count;
		x += delta_x;
	}

	return count;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< std::string > const lines{ std::istream_iterator< line_t >( ifs ), std::istream_iterator< line_t >() };

	std::cout
		<< "count = "
		<<    count_trees( lines, 1, 1 )
			* count_trees( lines, 3, 1 )
			* count_trees( lines, 5, 1 )
			* count_trees( lines, 7, 1 )
			* count_trees( lines, 1, 2 )
		<< std::endl;

	return 0;
}