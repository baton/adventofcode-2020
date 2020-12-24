#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>


size_t constexpr steps = 100;


struct field_t
{
	std::vector< int > v;

	field_t( std::string const & s )
	{
		for( char const c : s )
			v.push_back( c - '0' );
	}

	void step()
	{
		int const c( v[ 0 ] );
		int const c1( v[ 1 ] );
		int const c2( v[ 2 ] );
		int const c3( v[ 3 ] );

		std::vector< int > n( v );
		n.erase( n.begin() + 1, n.begin() + 4 );

		int d( c );
		if( --d == 0 )
			d = 9;
		while( ( d == c1 ) || ( d == c2 ) || ( d == c3 ) )
			if( --d == 0 )
				d = 9;
		
		std::vector< int >::const_iterator i( std::find( n.begin(), n.end(), d ) );
		n.insert( std::next( i ), { c1, c2, c3 } );
		n.push_back( c );
		v.assign( std::next( n.begin() ), n.end() );
	}

	std::string to_string() const
	{
		std::string s;
		std::vector< int >::const_iterator i( std::find( v.begin(), v.end(), 1 ) );
		for( std::vector< int >::const_iterator j( std::next( i ) ); j != v.end(); ++j )
			s.push_back( '0' + *j );
		for( std::vector< int >::const_iterator j( v.begin() ); j != i; ++j )
			s.push_back( '0' + *j );
		return s;
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

	std::cout << "result = " << field.to_string() << std::endl;

	return 0;
}