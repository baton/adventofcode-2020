#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <memory>


size_t fail_index( std::vector< uint64_t > const v, size_t const preamble_length )
{
	for( size_t i( preamble_length ); i != v.size(); ++i )
	{
		bool found( false );
		for( size_t j( i - preamble_length ); ( j != i ) && !found; ++j )
			for( size_t k( j + 1 ); ( k != i ) && !found; ++k )
				found = ( v[ i ] == ( v[ j ] + v[ k ] ) );

		if( !found )
			return i;
	}

	return 0;
}

std::pair< size_t, size_t > find_sum_span( std::vector< uint64_t > const v, uint64_t const x )
{
	for( size_t b( 0 ); b != v.size() - 1; ++b )
		for( size_t e( b + 1 ); e != v.size(); ++e )
			if( x == std::accumulate( v.begin() + b, v.begin() + e, 0 ) )
				return std::pair< size_t, size_t >( b, e );

	return std::pair< size_t, size_t >( 0, 0 );
}

uint64_t break_sum( std::vector< uint64_t > const v, uint64_t const x )
{
	std::pair< size_t, size_t > const sum_span( find_sum_span( v, x ) );
	if( sum_span.first == sum_span.second )
		throw std::logic_error( "empty sum span" );

	return *std::min_element( v.begin() + sum_span.first, v.begin() + sum_span.second )
		+ *std::max_element( v.begin() + sum_span.first, v.begin() + sum_span.second );
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< uint64_t > const v{ std::istream_iterator< uint64_t >( ifs ), std::istream_iterator< uint64_t >() };

	uint64_t const sum( break_sum( v, v[ fail_index( v, 25 ) ] ) );

	std::cout << "sum = " << sum << std::endl;

	return 0;
}