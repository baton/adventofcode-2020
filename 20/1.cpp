#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>


size_t constexpr tile_size = 10;

typedef std::array< size_t, 4 > tileimage_t;
typedef std::array< tileimage_t, 16 > tiledata_t;
typedef std::pair< size_t, tiledata_t > tile_t;
typedef std::map< size_t, tiledata_t > tiles_t;

tileimage_t constexpr zero = { 0, 0, 0, 0 };


inline size_t bitflip( size_t x )
{
	size_t r( 0 );
	for( size_t i( 0 ); i != tile_size; ++i, x >>= 1 )
		r = ( r << 1 ) | ( x & 1 );

	return r;
}

inline size_t putbit( size_t x, bool b )
{
	return ( x << 1 ) | ( b ? 1 : 0 );
}

inline size_t str_to_bits( std::string const & s )
{
	return std::transform_reduce( s.begin(), s.end(),
		0, 
		putbit,
		[]( char const c ) -> bool
		{
			return c == '#';
		}
	);
}


tileimage_t flip_v( tileimage_t const & im )
{
	return { im[ 2 ], bitflip( im[ 1 ] ), im[ 0 ], bitflip( im[ 3 ] ) };
}

tileimage_t flip_h( tileimage_t const & im )
{
	return { bitflip( im[ 0 ] ), im[ 3 ], bitflip( im[ 2 ] ), im[ 1 ] };
}

tileimage_t rotate_cv( tileimage_t const & im )
{
	return { bitflip( im[ 3 ] ), im[ 0 ], bitflip( im[ 1 ] ), im[ 2 ] };
}


struct image_t
{
	size_t const max_size;
	int const min_coord;
	int const max_coord;

	typedef std::pair< size_t, tileimage_t > piece_t;
	typedef std::vector< piece_t > data_t;
	data_t data;

	image_t( size_t const sz )
		: max_size( ( sz + 1 ) * 2 )
		, min_coord( -sz )
		, max_coord( sz )
		, data( max_size * max_size )
	{}

	piece_t & at( int const x, int const y )
	{
		return data[ ( x + max_coord ) * max_size + ( y + max_coord ) ];
	}
};


namespace std
{
	std::istream & operator>>( std::istream & is, tile_t & tile )
	{
		tile = tile_t();
		if( is.eof() )
			return is;

		std::string s;
		is >> s >> tile.first;
		is.ignore( 1 );
		std::getline( is, s );

		tileimage_t ti{ 0, 0, 0, 0 };
		for( size_t i( 0 ); std::getline( is, s ) && !s.empty(); ++i )
		{
			if( i == 0 )
				ti[ 0 ] = str_to_bits( s );
			ti[ 1 ] = putbit( ti[ 1 ], s.back() == '#' );
			ti[ 3 ] = putbit( ti[ 3 ], s.front() == '#' );
			ti[ 2 ] = str_to_bits( s );
		}

		tile.second[ 0 ] = ti;
		tile.second[ 1 ] = flip_v( ti );
		tile.second[ 2 ] = flip_h( ti );
		tile.second[ 3 ] = flip_h( tile.second[ 1 ] );
		for( size_t i( 1 ); i != 4; ++i )
			for( size_t j( 0 ); j != 4; ++j )
				tile.second[ i * 4 + j ] = rotate_cv( tile.second[ ( i - 1 ) * 4 + j ] );


		return is;
	}
}


struct matchdata_t
{
	int dx, dy;
	size_t mp, tp;
} const matchdata[] =
{
	{ -1, 0, 0, 2 },
	{ 0, 1, 1, 3 },
	{ 1, 0, 2, 0 },
	{ 0, -1, 3, 1 }
};


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	tiles_t tiles{ std::istream_iterator< tile_t >( ifs ), std::istream_iterator< tile_t >() };

	image_t image( tiles.size() );
	image.at( 0, 0 ) = image_t::piece_t( tiles.begin()->first, tiles.begin()->second[ 0 ] );
	tiles.erase( tiles.begin() );

	int xb( -1 ), xe( 2 ), yb( -1 ), ye( 2 );

	while( !tiles.empty() )
	{
		for( tiles_t::const_iterator i( tiles.begin() ); i != tiles.end(); ++i )
		{
			tile_t const & tile( *i );

			image_t::piece_t piece;

			bool fit( false );
			for( int x( xb ); !fit && x != xe; ++x )
				for( int y( yb ); !fit && ( y != ye ); ++y )
					if( image.at( x, y ).first == 0 )
					{
						for( size_t m( 0 ); !fit && ( m != 4 ); ++m )
						{
							image_t::piece_t const & p( image.at( x + matchdata[ m ].dx, y + matchdata[ m ].dy ) );

							if( p.first != 0 )
								for( size_t v( 0 ); !fit && ( v != tile.second.size() ); ++v )
									if( fit = ( tile.second[ v ][ matchdata[ m ].tp ] == p.second[ matchdata[ m ].mp ] ) )
										piece = image_t::piece_t( tile.first, tile.second[ v ] );
						}

						if( fit )
						{
							image.at( x, y ) = piece;
							if( x == xb )
								--xb;
							if( x == xe - 1 )
								++xe;
							if( y == yb )
								--yb;
							if( y == ye - 1 )
								++ye;
						}
					}

			if( fit )
			{
				i = tiles.erase( i );
				break;
			}
		}
	}

	uint64_t const result( 1ULL
		* image.at( xb + 1, yb + 1 ).first
		* image.at( xb + 1, ye - 2 ).first
		* image.at( xe - 2, yb + 1 ).first
		* image.at( xe - 2, ye - 2 ).first
	);

	std::cout << "result = " << result << std::endl;

	return 0;
}