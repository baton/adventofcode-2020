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
	int east = 0;
	int south = 0;
	int waypoint_east = 10;
	int waypoint_south = -1;

	position_t & move( char const dir, int const val )
	{
		switch( dir )
		{
			case 'R':
			case 'L':
				{
					int a( ( dir == 'R' ? 1 : -1 ) * val );
					while( a < 0 )
						a += 360;
					a %= 360;
					if( a == 90 )
					{
						int t( waypoint_east );
						waypoint_east = -waypoint_south;
						waypoint_south = t;
					}
					else if( a == 180 )
					{
						waypoint_east = -waypoint_east;
						waypoint_south = -waypoint_south;
					}
					else if( a == 270 )
					{
						int t( waypoint_east );
						waypoint_east = waypoint_south;
						waypoint_south = -t;
					}
				}
				break;
			case 'N':
				waypoint_south -= val;
				break;
			case 'S':
				waypoint_south += val;
				break;
			case 'E':
				waypoint_east += val;
				break;
			case 'W':
				waypoint_east -= val;
				break;
			case 'F':
				south += waypoint_south * val;
				east += waypoint_east * val;
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