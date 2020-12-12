#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <numeric>


typedef std::vector< std::vector< char > > field_t;

namespace std
{
	std::istream & operator>>( std::istream & is, std::vector< char > & v )
	{
		std::string s;
		is >> s;

		v.assign( s.begin(), s.end() );

		return is;
	}
}

size_t count_adjacent( field_t const & f, size_t const x, size_t const y )
{
	size_t c( 0 );
	if( x != 0 )
		c += f[ x - 1 ][ y ] == '#';
	if( y != 0 )
		c += f[ x ][ y - 1 ] == '#';
	if( x != f.size() - 1 )
		c += f[ x + 1 ][ y ] == '#';
	if( y != f[ x ].size() - 1 )
		c += f[ x ][ y + 1 ] == '#';
	if( ( x != 0 ) && ( y != 0 ) )
		c += f[ x - 1 ][ y - 1 ] == '#';
	if( ( x != 0 ) && ( y != f[ x ].size() - 1 ) )
		c += f[ x - 1 ][ y + 1 ] == '#';
	if( ( x != f.size() - 1 ) && ( y != 0 ) )
		c += f[ x + 1 ][ y - 1 ] == '#';
	if( ( x != f.size() - 1 ) && ( y != f[ x ].size() - 1 ) )
		c += f[ x + 1 ][ y + 1 ] == '#';

	return c;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	field_t field{ std::istream_iterator< std::vector< char > >( ifs ),
		std::istream_iterator< std::vector< char > >() };

	field_t next( field );

	do
	{
		field = next;

		for( size_t x( 0 ); x != field.size(); ++x )
			for( size_t y( 0 ); y != field[ x ].size(); ++y )
				if( field[ x ][ y ] != '.' )
				{
					size_t const c( count_adjacent( field, x, y ) );
					if( ( field[ x ][ y ] == 'L' ) && ( c == 0 ) )
						next[ x ][ y ] = '#';
					else if( ( field[ x ][ y ] == '#' ) && ( c >= 4 ) )
						next[ x ][ y ] = 'L';
				}

	} while( field != next );

	size_t const count(
		std::transform_reduce( field.begin(), field.end(),
			0,
			std::plus< size_t >(),
			[]( std::vector< char > const & v ) -> size_t
			{
				return std::count( v.begin(), v.end(), '#' );
			}
		)
	);

	std::cout << "count = " << count << std::endl;

	return 0;
}