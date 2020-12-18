#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;


template< typename Iterator >
struct calc_t : qi::grammar< Iterator, uint64_t(), ascii::space_type >
{
	calc_t()
		: calc_t::base_type( expr )
	{
		using boost::spirit::_val;
		using boost::spirit::_1;
		using qi::ulong_long;

		expr = term[ _val = _1 ]
			>> *( ( '+' >> term[ _val += _1 ] )
				| ( '*' >> term[ _val *= _1 ] ) );

		term = ulong_long
			| ( '(' >> expr >> ')' );
	}

	qi::rule< Iterator, uint64_t(), ascii::space_type > expr, term;
};


struct line_t : std::string
{
	operator uint64_t() const
	{
		static calc_t< std::string::const_iterator > calc;

		uint64_t r( 0 );

		std::string::const_iterator b{ begin() }, e{ end() };
		if( !qi::phrase_parse( b, e, calc, ascii::space, r ) )
			throw std::runtime_error( "bad input" );

		return r;
	}
};

std::istream & operator>>( std::istream & is, line_t & line )
{
	return std::getline( is, line );
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	uint64_t const sum( std::accumulate( std::istream_iterator< line_t >( ifs ), std::istream_iterator< line_t >(), 0ULL ) );

	std::cout << "sum = " << sum << std::endl;

	return 0;
}
