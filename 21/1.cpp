#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;


typedef std::pair< std::set< std::string >, std::set< std::string > > linedata_t;

template< typename Iterator >
struct line_grammar_t : qi::grammar< Iterator, linedata_t(), ascii::space_type >
{
	line_grammar_t()
		: line_grammar_t::base_type( line )
	{
		using qi::char_;
		using qi::lit;

		line = +word >> lit( "(contains " ) >> ( word % ',' ) >> lit( ')' );

		word = +char_( 'a', 'z' );
	}

	qi::rule< Iterator, linedata_t(), ascii::space_type > line;
	qi::rule< Iterator, std::string() > word;
};


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< std::string > menu;

	typedef std::map< std::string, std::set< std::string > > rev_table_t;
	rev_table_t a_in_i;


	for( std::string s; std::getline( ifs, s ) && !s.empty(); )
	{
		static line_grammar_t< std::string::const_iterator > lg;

		linedata_t linedata;

		std::string::const_iterator b{ s.begin() }, e{ s.end() };
		if( !qi::phrase_parse( b, e, lg, ascii::space, linedata ) || ( b != e ) )
			throw std::runtime_error( "bad input for line" );

		menu.insert( menu.end(), linedata.first.begin(), linedata.first.end() );

		for( std::string const & a : linedata.second )
		{
			rev_table_t::iterator i( a_in_i.find( a ) );
			if( i != a_in_i.end() )
			{
				std::set< std::string > n;
				std::set_intersection(
					i->second.begin(), i->second.end(),
					linedata.first.begin(), linedata.first.end(),
					std::inserter< std::set< std::string > >( n, n.end() ) );
				i->second = std::move( n );
			}
			else
				a_in_i.emplace( a, linedata.first );
		}
	}

	std::set< std::string > const bad{
		std::reduce( a_in_i.begin(), a_in_i.end(),
			std::set< std::string >(),
			[]( std::set< std::string > & b, rev_table_t::value_type const & i ) -> std::set< std::string >
			{
				b.insert( i.second.begin(), i.second.end() );
				return std::move( b );
			}
		)
	};

	size_t const count(
		std::count_if( menu.begin(), menu.end(),
			[ &bad ]( std::string const & i ) -> bool
			{
				return bad.count( i ) == 0;
			}
		)
	);

	std::cout << "count = " << count << std::endl;

	return 0;
}