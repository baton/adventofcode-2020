#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <iterator>


struct id_t
{
	size_t n;

	operator size_t() const
	{
		return n;
	}
};

std::istream & operator>>( std::istream & is, id_t & id )
{
	std::string s;
	is >> s;

	id.n = std::accumulate( s.begin(), s.end(),
		0,
		[]( size_t const n, char const c ) -> size_t
		{
			return ( n << 1 )
				+ ( ( c == 'B' ) || ( c == 'R' ) ? 1 : 0 );
		}
	);

	return is;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< size_t > v{ std::istream_iterator< id_t >( ifs ), std::istream_iterator< id_t >() };
	std::sort( v.begin(), v.end() );

	size_t const id( 1 + *std::adjacent_find( v.begin(), v.end(),
		[]( size_t const a, size_t const b ) -> bool
		{
			return ( b - a ) > 1;
		}	
	) );

	std::cout << "ID = " << id << std::endl;

	return 0;
}