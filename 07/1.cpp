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


typedef std::map< std::string, std::vector< std::string > > rules_t;
typedef std::pair< std::string, std::vector< std::string > > rule_t;

template< typename Iterator >
struct rule_grammar_t : qi::grammar< Iterator, rule_t() >
{
	rule_grammar_t()
		: rule_grammar_t::base_type( rule )
	{
		using qi::char_;
		using qi::int_;
		using qi::lit;

		rule = colour >> lit( " contain " ) >> contains >> lit( '.' );

		colour = +char_( 'a', 'z' ) >> char_( ' ' ) >> +char_( 'a', 'z' ) >> lit( " bag" ) >> *lit( 's' );
		item = qi::omit[ int_ >> lit( ' ' ) ] >> colour;
		contains = ( item % ", " ) | lit( "no other bags" );
	}

	qi::rule< Iterator, rule_t() > rule;
	qi::rule< Iterator, std::string() > colour, item;
	qi::rule< Iterator, std::vector< std::string > () > contains;

};

namespace std
{
	std::istream & operator>>( std::istream & is, rule_t & r )
	{
		static rule_grammar_t< std::string::const_iterator > rg;

		std::string s;
		std::getline( is, s );

		if( !s.empty() )
		{
			r = rule_t();
			std::string::const_iterator b{ s.begin() }, e{ s.end() };
			if( !qi::phrase_parse( b, e, rg, qi::space, r ) || ( b != e ) )
				throw std::runtime_error( "bad input for rule_t" );
		}

		return is;
	}
}

bool ends_with_target( rules_t const & rules, rule_t const & rule, std::string const & target )
{
	for( std::vector< std::string >::const_iterator i{ rule.second.begin() }; i != rule.second.end(); ++i )
		if( ( *i == target ) || ends_with_target( rules, std::make_pair( *i, rules.at( *i ) ), target ) )
			return true;

	return false;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	rules_t const rules{ std::istream_iterator< rule_t >( ifs ), std::istream_iterator< rule_t >() };

	size_t const count(
		std::count_if( rules.begin(), rules.end(),
			[ &rules ]( rule_t const & rule ) -> bool
			{
				return ends_with_target( rules, rule, "shiny gold" );
			}
		)
	);

	std::cout << "count = " << count << std::endl;

	return 0;
}