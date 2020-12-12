#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>


inline bool is_diff_3( uint64_t const a, uint64_t const b )
{
	return a + 3 == b;
}

uint64_t count_distinct_ways( std::vector< uint64_t >::const_iterator b, std::vector< uint64_t >::const_iterator e )
{
	if( b + 1 == e )
		return 1;

	uint64_t const x( *b );

	uint64_t n( 0 );
	for( uint64_t o( 1 ); ( o != 4 ) && ( b != e ) && ( *( b + o ) - x <= 3 ); ++o )
		n += count_distinct_ways( b + o, e );

	return n;
}

uint64_t count_distinct_ways( std::vector< uint64_t > const & v )
{
	uint64_t c( 1 );

	std::vector< uint64_t >::const_iterator b{ v.begin() }, e{ b };
	for( e = std::adjacent_find( e, v.end(), is_diff_3 ); e != v.end(); b = e + 1, e = std::adjacent_find( b, v.end(), is_diff_3 ) )
		c *= count_distinct_ways( b, e + 1 );

	return c;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< uint64_t > v{ std::istream_iterator< uint64_t >( ifs ), std::istream_iterator< uint64_t >() };
	v.push_back( 0 );
	std::sort( v.begin(), v.end() );
	v.push_back( v.back() + 3 );

	uint64_t const count( count_distinct_ways( v ) );

	std::cout << "count = " << count << std::endl;

	return 0;
}