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
			for( size_t k( j ); k != v.size(); ++k )
				if( v[ i ] + v[ j ] + v[ k ] == 2020 )
				{
					std::cout << "2020 = " << v[ i ] << " + " << v[ j ] << " + " << v[ k ] << std::endl
						<< v[ i ] << " * " << v[ j ] << " * " << v[ k ] << " = " << v[ i ] * v[ j ] * v[ k ] << std::endl;
					return 0;
				}
	
	std::cout << "nope" << std::endl;

	return 0;
}