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

typedef std::pair< std::array< size_t, 4 >, std::vector< std::string > > tileimage_t;
typedef std::array< tileimage_t, 16 > tiledata_t;
typedef std::pair< size_t, tiledata_t > tile_t;
typedef std::map< size_t, tiledata_t > tiles_t;

tileimage_t const zero = { { 0, 0, 0, 0 }, std::vector< std::string >() };


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


std::vector< std::string > flip_v( std::vector< std::string > const & v )
{
	return { v.rbegin(), v.rend() };
}

tileimage_t flip_v( tileimage_t const & im )
{
	return { 
		{ im.first[ 2 ], bitflip( im.first[ 1 ] ), im.first[ 0 ], bitflip( im.first[ 3 ] ) },
		flip_v( im.second )
	};
}

std::vector< std::string > flip_h( std::vector< std::string > const & v )
{
	std::vector< std::string > r;
	std::transform( v.begin(), v.end(), std::back_inserter< std::vector< std::string > >( r ),
		[]( std::string const & s ) -> std::string
		{
			return { s.rbegin(), s.rend() };
		}
	);
	return r;
}

tileimage_t flip_h( tileimage_t const & im )
{
	return {
		{ bitflip( im.first[ 0 ] ), im.first[ 3 ], bitflip( im.first[ 2 ] ), im.first[ 1 ] },
		flip_h( im.second )
	};
}

std::vector< std::string > rotate_cv( std::vector< std::string > const & v )
{
	std::vector< std::string > r{ v.front().length(), "" };
	for( std::vector< std::string >::const_reverse_iterator i( v.crbegin() ); i != v.crend(); ++i )
		for( size_t j( 0 ); j != i->length(); ++j )
			r[ j ].push_back( i->at( j ) );
	return r;
}

tileimage_t rotate_cv( tileimage_t const & im )
{
	return {
		{ bitflip( im.first[ 3 ] ), im.first[ 0 ], bitflip( im.first[ 1 ] ), im.first[ 2 ] },
		rotate_cv( im.second )
	};
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

		if( !tile.first )
			return is;

		tileimage_t ti( zero );
		for( size_t i( 0 ); std::getline( is, s ) && !s.empty(); ++i )
		{
			if( i == 0 )
				ti.first[ 0 ] = str_to_bits( s );
			ti.first[ 1 ] = putbit( ti.first[ 1 ], s.back() == '#' );
			ti.first[ 3 ] = putbit( ti.first[ 3 ], s.front() == '#' );
			ti.first[ 2 ] = str_to_bits( s );
			ti.second.push_back( s );
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
									if( fit = ( tile.second[ v ].first[ matchdata[ m ].mp ] == p.second.first[ matchdata[ m ].tp ] ) )
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


	++xb;
	--xe;
	++yb;
	--ye;

	std::vector< std::string > picture( ( xe - xb ) * ( tile_size - 2 ), "" );
	for( int x( xb ); x != xe; ++x )
		for( int y( yb ); y != ye; ++y )
		{
			image_t::piece_t const & p( image.at( x, y ) );
			for( size_t l( 1 ); l != p.second.second.size() - 1; ++l )
				picture.at( ( x - xb ) * ( tile_size - 2 ) + l - 1 ).append( p.second.second[ l ].substr( 1, 8 ) );
		}

	std::array< std::string, 3 > const monster{
		"                  # ",
		"#    ##    ##    ###",
		" #  #  #  #  #  #   "
	};
	size_t const monster_height( monster.size() );
	size_t const monster_width( monster[ 0 ].length() );

	auto match_monster =
		[ &monster, &monster_height, &monster_width ]
		( std::vector< std::string > const & p, size_t const x, size_t const y ) -> bool
	{
		for( size_t i( 0 ); i != monster_height; ++i )
			for( size_t j( 0 ); j != monster_width; ++j )
				if( ( monster[ i ][ j ] == '#' ) && ( p[ x + i ][ y + j ] != '#' ) )
					return false;
		return true;
	};
	auto replace_monster =
		[ &monster, &monster_height, &monster_width ]
		( std::vector< std::string > & p, size_t const x, size_t const y ) -> void
	{
		for( size_t i( 0 ); i != monster_height; ++i )
			for( size_t j( 0 ); j != monster_width; ++j )
				if( monster[ i ][ j ] == '#' )
					p[ x + i ][ y + j ] = 'O';
	};

	std::array< std::vector< std::string >, 16 > pics;
	pics[ 0 ] = picture;
	pics[ 1 ] = flip_v( picture );
	pics[ 2 ] = flip_h( picture );
	pics[ 3 ] = flip_h( pics[ 1 ] );
	for( size_t i( 1 ); i != 4; ++i )
		for( size_t j( 0 ); j != 4; ++j )
			pics[ i * 4 + j ] = rotate_cv( pics[ ( i - 1 ) * 4 + j ] );

	for( size_t i( 0 ); i != pics.size(); ++i )
	{
		std::vector< std::string > & p( pics[ i ] );

		bool monsters( false );
		for( size_t x( 0 ); x != p.size() - monster_height; ++x )
			for( size_t y( 0 ); y != p[ x ].length() - monster_width; ++y )
				if( match_monster( p, x, y ) )
				{
					monsters = true;
					replace_monster( p, x, y );
				}
		if( monsters )
		{
			size_t const count(
				std::transform_reduce( p.begin(), p.end(),
					0,
					std::plus(),
					[]( std::string const & s ) -> size_t
					{
						return std::count( s.begin(), s.end(), '#' );
					}
				)
			);

			std::cout << "count = " << count << std::endl;

			return 0;
		}
	}

	return 0;
}