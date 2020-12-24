#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <algorithm>


size_t constexpr steps = 10000000;
int constexpr max_cup = 1000000;


struct field_t
{
	typedef std::list< int > list_t;
	list_t v;
	typedef std::map< int, list_t::const_iterator > map_t;
	map_t m;

	field_t( std::string const & s )
	{
		for( char const c : s )
		{
			int const x( c - '0' );
			m.insert( { x, v.insert( v.end(), x ) } );
		}
		for( int x( 10 ); x != max_cup + 1; ++x )
			m.insert( { x, v.insert( v.end(), x ) } );
	}

	void step()
	{
		int const c( v.front() );
		int const c1( *std::next( v.begin() ) );
		v.erase( std::next( v.begin() ) );
		int const c2( *std::next( v.begin() ) );
		v.erase( std::next( v.begin() ) );
		int const c3( *std::next( v.begin() ) );
		v.erase( std::next( v.begin() ) );

		int d( c );
		if( --d == 0 )
			d = max_cup;
		while( ( d == c1 ) || ( d == c2 ) || ( d == c3 ) )
			if( --d == 0 )
				d = max_cup;
		
		std::list< int >::const_iterator i( m[ d ] );
		m[ c1 ] = v.insert( std::next( i ), c1 );
		m[ c2 ] = v.insert( std::next( m[ c1 ] ), c2 );
		m[ c3 ] = v.insert( std::next( m[ c2 ] ), c3 );
		m[ c ] = v.insert( v.end(), c );
		v.pop_front();
	}

	uint64_t result() const
	{
		uint64_t r{ 1ULL };
		std::list< int >::const_iterator i( m.at( 1 ) );
		for( size_t j( 0 ); j != 2; ++j )
		{
			i = std::next( i );
			if( i == v.end() )
				i = v.begin();
			r *= *i;
		}
		return r;
	}
};


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );
	std::string s;
	ifs >> s;

	field_t field( s );
	for( size_t i( 0 ); i != steps; ++i )
		field.step();

	std::cout << "result = " << field.result() << std::endl;

	return 0;
}