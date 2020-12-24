#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include <numeric>


size_t constexpr steps = 100;


typedef std::array< int, 3 > pos_t;
typedef std::map< pos_t, size_t > tiles_t;


inline pos_t & normalize( pos_t & d )
{
	pos_t dd{ 0, 0, 0 };
	while( d != dd )
	{
		dd = d;
		while( ( d[ 1 ] > 0 ) && ( d[ 2 ] > 0 ) )
		{
			++d[ 0 ];
			--d[ 1 ];
			--d[ 2 ];
		}
		while( ( d[ 1 ] < 0 ) && ( d[ 2 ] < 0 ) )
		{
			--d[ 0 ];
			++d[ 1 ];
			++d[ 2 ];
		}
		while( ( d[ 0 ] > 0 ) && ( d[ 1 ] < 0 ) )
		{
			--d[ 0 ];
			++d[ 1 ];
			++d[ 2 ];
		}
		while( ( d[ 0 ] < 0 ) && ( d[ 1 ] > 0 ) )
		{
			++d[ 0 ];
			--d[ 1 ];
			--d[ 2 ];
		}
		while( ( d[ 0 ] > 0 ) && ( d[ 2 ] < 0 ) )
		{
			--d[ 0 ];
			++d[ 1 ];
			++d[ 2 ];
		}
		while( ( d[ 0 ] < 0 ) && ( d[ 2 ] > 0 ) )
		{
			++d[ 0 ];
			--d[ 1 ];
			--d[ 2 ];
		}
	}

	return d;
}

inline pos_t operator+( pos_t const & a, pos_t const & b )
{
	return normalize( pos_t{ a[ 0 ] + b[ 0 ], a[ 1 ] + b[ 1 ], a[ 2 ] + b[ 2 ] } );
}

inline pos_t & operator+=( pos_t & a, pos_t const & b )
{
	a[ 0 ] += b[ 0 ];
	a[ 1 ] += b[ 1 ];
	a[ 2 ] += b[ 2 ];
	return normalize( a );
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	tiles_t tiles;

	for( std::string l; std::getline( ifs, l ); )
	{
		pos_t d{ 0, 0, 0 };

		std::string::const_iterator b{ l.begin() }, e{ l.end() };
		bool n( false ), s( false );
		while( b != e )
		{
			switch( *b )
			{
				case 'e':
					if( n )
						++d[ 1 ];
					else if( s )
						++d[ 2 ];
					else
						++d[ 0 ];
					n = s = false;
					break;
				case 'w':
					if( n )
						--d[ 2 ];
					else if( s )
						--d[ 1 ];
					else
						--d[ 0 ];
					n = s = false;
					break;
				case 'n':
					n = true;
					break;
				case 's':
					s = true;
					break;
			}
			++b;
		}

		++tiles[ normalize( d ) ];
	}

	tiles_t next;
	for( tiles_t::const_iterator t( tiles.begin() ); t != tiles.end(); ++t )
	{
		std::array< pos_t, 6 > const d{
			pos_t{ -1, 0, 0 },
			{ 1, 0, 0 },
			{ 0, -1, 0 },
			{ 0, 1, 0 },
			{ 0, 0, -1 },
			{ 0, 0, 1 }
		};

		next[ t->first ] = t->second % 2;
		for( size_t i( 0 ); i != d.size(); ++i )
		{
			pos_t const dd( t->first + d[ i ] );
			if( !tiles.count( dd ) )
				next[ dd ] = 0;
		}
	}
	tiles = next;

	for( size_t step( 0 ); step != steps; ++step )
	{
		next = tiles_t();

		for( tiles_t::const_iterator t( tiles.begin() ); t != tiles.end(); ++t )
		{
			std::array< pos_t, 6 > const d{
				pos_t{ -1, 0, 0 },
				{ 1, 0, 0 },
				{ 0, -1, 0 },
				{ 0, 1, 0 },
				{ 0, 0, -1 },
				{ 0, 0, 1 }
			};

			size_t n( 0 );
			for( size_t i( 0 ); i != d.size(); ++i )
			{
				pos_t const dd( t->first + d[ i ] );
				tiles_t::const_iterator ti( tiles.find( dd ) );
				if( ti != tiles.end() )
					n += ti->second % 2;
				else
					next[ dd ] = 0;
			}

			size_t f( t->second );
			if( ( ( t->second % 2 ) != 0 ) && ( ( n == 0 ) || ( n > 2 ) ) )
				f = 0;
			else if( ( ( t->second % 2 ) == 0 ) && ( n == 2 ) )
				f = 1;
			next[ t->first ] = f;
		}

		tiles = std::move( next );

		size_t const count(
			std::count_if( tiles.begin(), tiles.end(),
				[]( std::map< pos_t, size_t >::value_type const & t ) -> bool
		{
			return ( t.second % 2 ) != 0;
		}
			)
		);
	}


	size_t const count(
		std::count_if( tiles.begin(), tiles.end(), 
			[]( std::map< pos_t, size_t >::value_type const & t ) -> bool
			{
				return ( t.second % 2 ) != 0;
			}
		)
	);

	std::cout << "count = " << count << std::endl;

	return 0;
}