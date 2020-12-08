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


typedef std::pair< size_t, std::string > rule_value_t;
typedef std::map< std::string, std::vector< rule_value_t > > rules_t;
typedef std::pair< std::string, std::vector< rule_value_t > >  rule_t;

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
		item = int_ >> lit( ' ' ) >> colour;
		contains = ( item % ", " ) | lit( "no other bags" );
	}

	qi::rule< Iterator, rule_t() > rule;
	qi::rule< Iterator, std::string() > colour;
	qi::rule< Iterator, rule_value_t() > item;
	qi::rule< Iterator, std::vector< rule_value_t >() > contains;

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

size_t count_bags( rules_t const & rules, std::string const & target )
{
	return std::accumulate( rules.at( target ).begin(), rules.at( target ).end(),
		1,
		[ &rules ]( size_t const c, rule_value_t const & rule_value ) -> size_t
		{
			return c + rule_value.first * count_bags( rules, rule_value.second );
		}
	);
}

int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	rules_t const rules{ std::istream_iterator< rule_t >( ifs ), std::istream_iterator< rule_t >() };

	size_t const count( count_bags( rules, "shiny gold" ) - 1 );

	std::cout << "count = " << count << std::endl;

	return 0;
}