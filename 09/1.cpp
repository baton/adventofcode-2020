#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< uint64_t > const v{ std::istream_iterator< uint64_t >( ifs ), std::istream_iterator< uint64_t >() };

	for( size_t i( 25 ); i != v.size(); ++i )
	{
		bool found( false );
		for( size_t j( i - 25 ); ( j != i ) && !found; ++j )
			for( size_t k( j + 1 ); ( k != i ) && !found; ++k )
				found = ( v[ i ] == ( v[ j ] + v[ k ] ) );

		if( !found )
		{
			std::cout << "n = " << v[ i ] << std::endl;

			break;
		}
	}

	return 0;
}