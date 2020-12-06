#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <functional>


typedef std::vector< size_t > group_answer_t;

namespace std
{
	std::istream & operator>>( std::istream & is, group_answer_t & ga )
	{
		ga.clear();
	
		std::string s;
		for( std::getline( is, s ); !s.empty(); std::getline( is, s ) )
		{
			ga.push_back(
				std::accumulate( s.begin(), s.end(),
					0,
					[]( size_t const n, char const c ) -> size_t
					{
						return n | ( 1 << ( c - 'a' ) );
					}
				)
			);

			if( is.eof() )
				break;
		}
	
		return is;
	}
}

inline size_t count_bits( size_t x )
{
	size_t c( 0 );
	while( x )
	{
		++c;
		x &= x - 1;
	}
	return c;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	size_t const count(
		std::accumulate( std::istream_iterator< group_answer_t >( ifs ), std::istream_iterator< group_answer_t >(),
			0,
			[]( size_t const c, group_answer_t const & ga ) -> size_t
			{
				return c
					+ count_bits( std::accumulate( ga.begin(), ga.end(),
						std::numeric_limits< size_t >::max(), std::bit_and< size_t >() ) );
			}
		)
	);

	std::cout << "count = " << count << std::endl;

	return 0;
}