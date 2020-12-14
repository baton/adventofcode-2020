#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include <boost/spirit/include/qi.hpp>

namespace qi = boost::spirit::qi;


template< typename Iterator >
struct timetable_grammar_t : qi::grammar< Iterator, std::vector< size_t >() >
{
	timetable_grammar_t()
		: timetable_grammar_t::base_type( timetable )
	{
		using qi::uint_;
		using qi::lit;

		timetable = ( uint_ | qi::omit[ lit( 'x' ) ] ) % ',';
	}

	qi::rule< Iterator, std::vector< size_t >() > timetable;
};

namespace std
{
	std::istream & operator>>( std::istream & is, std::vector< size_t > & v )
	{
		static timetable_grammar_t< std::string::const_iterator > ttg;

		std::string s;
		std::getline( is, s );

		if( !s.empty() )
		{
			v.clear();
			std::string::const_iterator b{ s.begin() }, e{ s.end() };
			if( !qi::phrase_parse( b, e, ttg, qi::space, v ) || ( b != e ) )
				throw std::runtime_error( "bad input for timetable" );
		}

		return is;
	}
}


size_t time_to_next( size_t const arrival, size_t const schedule )
{
	return schedule - ( arrival % schedule );
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	size_t arrival_time( 0 );
	std::vector< size_t > timetable;
	ifs >> arrival_time >> std::ws >> timetable;


	for( uint64_t t( 0 ), n( 0 ), s( 1 ); t != std::numeric_limits< uint64_t >::max() && ( n != timetable.size() ); t += s )
	{
		for( ; n != timetable.size(); ++n )
		{
			if( timetable[ n ] != 0 )
			{
				if( ( t + n ) % timetable[ n ] == 0 )
					s *= timetable[ n ];
				else
					break;
			}
		}

		if( n == timetable.size() )
		{
			std::cout << "result = " << t << std::endl;

			return 0;
		}
	}

	return 0;
}