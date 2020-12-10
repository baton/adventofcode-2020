#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< uint64_t > v{ std::istream_iterator< uint64_t >( ifs ), std::istream_iterator< uint64_t >() };
	v.push_back( 0 );
	std::sort( v.begin(), v.end() );
	v.push_back( v.back() + 3 );

	std::adjacent_difference( v.begin(), v.end(), v.begin() );

	size_t const count_1( std::count( v.begin(), v.end(), 1 ) );
	size_t const count_3( std::count( v.begin(), v.end(), 3 ) ); 

	std::cout << "count_1 * count_3 = " << count_1 * count_3 << std::endl;

	return 0;
}