#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;


typedef std::vector< size_t > rulelist_t;
typedef std::pair< char, std::vector< rulelist_t > > rule_t;
typedef std::map< size_t, rule_t > ruleset_t;


template< typename Iterator >
struct rule_grammar_t : qi::grammar< Iterator, std::pair< size_t, rule_t >(), ascii::space_type >
{
	rule_grammar_t()
		: rule_grammar_t::base_type( line )
	{
		using qi::uint_;
		using qi::char_;
		using qi::lit;
		using qi::attr;

		line = uint_ >> lit( ':' ) >> rule;

		rule = ( ( lit( '"' ) >> char_ >> lit( '"' ) ) >> attr( std::vector< std::vector< size_t > >() ) )
			| attr( '\0' ) >> ( list % '|' );

		list = +uint_;
	}

	qi::rule< Iterator, std::pair< size_t, rule_t >(), ascii::space_type > line;
	qi::rule< Iterator, rule_t(), ascii::space_type > rule;
	qi::rule< Iterator, std::vector< size_t >(), ascii::space_type > list;
};

bool match( ruleset_t const & rules, rulelist_t::const_iterator rb, rulelist_t::const_iterator const & re,
	std::string::const_iterator sb, std::string::const_iterator const & se )
{
	while( ( rb != re ) && ( sb != se ) )
	{
		rule_t const & r( rules.at( *rb ) );
		if( !r.second.empty() )
		{
			for( size_t i( 0 ); i != r.second.size(); ++i )
			{
				rulelist_t rl( r.second[ i ] );
				rl.insert( rl.end(), std::next( rb ), re );
				if( match( rules, rl.begin(), rl.end(), sb, se ) )
					return true;
			}
			return false;
		}
		else if( *sb == r.first )
		{
			++rb;
			++sb;
		}
		else
			return false;
	}

	return ( ( rb == re ) && ( sb == se ) );
}


bool match( ruleset_t const & rules, std::string const & s )
{
	rulelist_t const start{ 0 };

	return match( rules, start.begin(), start.end(), s.begin(), s.end() );
}



int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	ruleset_t rules;

	for( std::string s; std::getline( ifs, s ) && !s.empty(); )
	{
		static rule_grammar_t< std::string::const_iterator > rg;

		std::pair< size_t, rule_t > rl;
		std::string::const_iterator b{ s.begin() }, e{ s.end() };
		if( !qi::phrase_parse( b, e, rg, ascii::space, rl ) || ( b != e ) )
			throw std::runtime_error( "bad input for rule" );

		rules.insert( std::move( rl ) );
	}

	size_t const count(
		std::count_if( std::istream_iterator< std::string >( ifs ), std::istream_iterator< std::string >(),
			[ &rules ]( std::string const & s ) -> bool
			{
				return match( rules, s );
			}
		)
	);

	std::cout << "count = " << count << std::endl;

	return 0;
}