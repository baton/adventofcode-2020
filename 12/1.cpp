#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <memory>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

namespace qi = boost::spirit::qi;


enum class azimuth_t : int { NORTH = 0, EAST = 90, SOUTH = 180, WEST = 270 };
struct position_t
{
	azimuth_t azimuth = azimuth_t::EAST;
	int east = 0;
	int south = 0;

	inline char direction() const
	{
		switch( azimuth )
		{
			case azimuth_t::NORTH: return 'N';
			case azimuth_t::SOUTH: return 'S';
			case azimuth_t::EAST: return 'E';
			case azimuth_t::WEST: return 'W';
		}

		return '\0';
	}

	position_t & move( char const dir, int const val )
	{
		switch( dir )
		{
			case 'R':
			case 'L':
				{
					int a( static_cast< int >( azimuth ) + ( dir == 'R' ? 1 : -1 ) * val );
					while( a < 0 )
						a += 360;
					azimuth = static_cast< azimuth_t >( a % 360 );
				}
				break;
			case 'F':
				return move( direction(), val );
			case 'N':
				south -= val;
				break;
			case 'S':
				south += val;
				break;
			case 'E':
				east += val;
				break;
			case 'W':
				east -= val;
				break;
		}

		return *this;
	}
};

typedef std::pair< char, int > command_t;

template< typename Iterator >
struct command_grammar_t : qi::grammar< Iterator, command_t() >
{
	command_grammar_t()
		: command_grammar_t::base_type( command )
	{
		using qi::char_;
		using qi::int_;

		command = qi::char_( "NSEWFRL" ) >> int_;
	}

	qi::rule< Iterator, command_t() > command;
};

namespace std
{
	std::istream & operator>>( std::istream & is, command_t & command )
	{
		static command_grammar_t< std::string::const_iterator > cg;

		std::string s;
		std::getline( is, s );

		if( !s.empty() )
		{
			command = command_t();
			std::string::const_iterator b{ s.begin() }, e{ s.end() };
			if( !qi::phrase_parse( b, e, cg, qi::space, command ) || ( b != e ) )
				throw std::runtime_error( "bad input for command_t" );
		}

		return is;
	}
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	position_t const position(
		std::accumulate( std::istream_iterator< command_t >( ifs ), std::istream_iterator< command_t >(),
			position_t(),
			[]( position_t & p, command_t const & c ) -> position_t
			{
				return p.move( c.first, c.second );
			}
		)
	);

	std::cout << "result = " << std::abs( position.east ) + std::abs( position.south ) << std::endl;

	return 0;
}