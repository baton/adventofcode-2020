#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< int > const v{ std::istream_iterator< int >( ifs ), std::istream_iterator< int >() };

	for( size_t i( 0 ); i != v.size(); ++i )
		for( size_t j( i ); j != v.size(); ++j )
			if( v[ i ] + v[ j ] == 2020 )
			{
				std::cout << "2020 = " << v[ i ] << " + " << v[ j ] << std::endl
					<< v[ i ] << " * " << v[ j ] << " = " << v[ i ] * v[ j ] << std::endl;
				return 0;
			}

	return 0;
}