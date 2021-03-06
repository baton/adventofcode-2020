#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>


size_t constexpr steps = 6;

size_t constexpr space_size = 8;
size_t constexpr offs = steps + 1;

size_t constexpr space_width = space_size + offs * 2;
size_t constexpr space_height = space_size + offs * 2;
size_t constexpr space_depth = 1 + offs * 2;


typedef std::vector< char > space_t;

inline size_t coord( int const x, int const y, int const z )
{
	return ( x + offs ) + ( y + offs ) * space_width + ( z + offs ) * space_width * space_height;
}

inline char & in_space( space_t & space, int const x, int const y, int const z )
{
	return space[ coord( x, y, z ) ];
}


size_t count_heighbours( space_t & space, int const x, int const y, int const z )
{
	size_t c( 0 );

	for( int dx( -1 ); dx != 2; ++dx )
		for( int dy( -1 ); dy != 2; ++dy )
			for( int dz( -1 ); dz != 2; ++dz )
				if( ( ( dx != 0 ) || ( dy != 0 ) || ( dz != 0 ) )
					&& ( in_space( space, x + dx, y + dy, z + dz ) == '#' ) )
					++c;

	return c;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	space_t space( space_width * space_height * space_depth, '.' );

	int xb( 0 ), xe( 0 ), yb( 0 ), ye( 0 ), zb( 0 ), ze( 0 );

	for( std::string s; std::getline( ifs, s ); )
	{
		for( xe = 0; xe != s.length(); ++xe )
			in_space( space, xe, ye, ze ) = s[ xe ];
		++ye;
	}
	++ze;

	for( size_t step( 0 ); step != steps; ++step )
	{
		--xb; ++xe;
		--yb; ++ye;
		--zb; ++ze;

		space_t next( space_width * space_height * space_depth, '.' );

		for( int x( xb ); x != xe; ++x )
			for( int y( yb ); y != ye; ++y )
				for( int z( zb ); z != ze; ++z )
				{
					size_t const cnt( count_heighbours( space, x, y, z ) );
					char const c( in_space( space, x, y, z ) );
					char n( '.' );

					if( cnt == 3 )
						n = '#';
					else if( ( c == '#' ) && ( cnt == 2 ) )
						n = '#';

					in_space( next, x, y, z ) = n;
				}

		space = next;
	}

	size_t const count( std::count( space.begin(), space.end(), '#' ) );

	std::cout << "count = " << count << std::endl;

	return 0;
}