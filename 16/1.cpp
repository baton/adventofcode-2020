#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

namespace qi = boost::spirit::qi;


typedef std::pair< size_t, size_t > diap_t;
typedef std::pair< diap_t, diap_t > rule_t;
typedef std::vector< size_t > ticket_t;


template< typename Iterator >
struct rule_grammar_t : qi::grammar< Iterator, rule_t() >
{
	rule_grammar_t()
		: rule_grammar_t::base_type( rule )
	{
		using qi::uint_;
		using qi::char_;
		using qi::lit;

		rule = qi::omit[ +( char_( 'a', 'z' ) | char_( ' ' ) ) ] >> lit( ": " ) >> diap >> lit( " or " ) >> diap;
		diap = uint_ >> lit( '-' ) >> uint_;
	}

	qi::rule< Iterator, rule_t() > rule;
	qi::rule< Iterator, diap_t() > diap;
};


namespace std
{
	std::istream & operator>>( std::istream & is, ticket_t & t )
	{
		static qi::rule< std::string::const_iterator, ticket_t() > tg{ qi::uint_ % ',' };

		std::string s;
		std::getline( is, s );

		if( !s.empty() )
		{
			t.clear();
			std::string::const_iterator b{ s.begin() }, e{ s.end() };
			if( !qi::phrase_parse( b, e, tg, qi::space, t ) || ( b != e ) )
				throw std::runtime_error( "bad input for ticket" );
		}

		return is;
	}
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::vector< rule_t > rules;

	for( std::string s; std::getline( ifs, s ) && !s.empty(); )
	{
		static rule_grammar_t< std::string::const_iterator > rg;

		rule_t r;
		std::string::const_iterator b{ s.begin() }, e{ s.end() };
		if( !qi::phrase_parse( b, e, rg, qi::space, r ) || ( b != e ) )
			throw std::runtime_error( "bad input for ticket" );

		rules.push_back( std::move( r ) );
	}

	std::getline( ifs, std::string() ); // "your ticket:"
	std::getline( ifs, std::string() ); // ticket_t
	std::getline( ifs, std::string() ); // empty line
	std::getline( ifs, std::string() ); // "nearby tickets:"

	size_t const error_rate(
		std::transform_reduce( std::istream_iterator< ticket_t >( ifs ), std::istream_iterator< ticket_t >(),
			0,
			std::plus(),
			[ &rules ]( ticket_t const & t ) -> size_t
			{
				return std::accumulate( t.begin(), t.end(),
					0,
					[ &rules ]( size_t const sum, size_t const v ) -> size_t
					{
						bool const found(
							std::find_if( rules.begin(), rules.end(),
								[ &v ]( rule_t const & r ) -> bool
								{
									return ( ( v >= r.first.first ) && ( v <= r.first.second ) )
										|| ( ( v >= r.second.first ) && ( v <= r.second.second ) );
								}
							) != rules.end()
						);
						return sum + ( !found ? v : 0 );
					}
				);
			}
		)
	);

	std::cout << "error rate = " << error_rate << std::endl;

	return 0;
}