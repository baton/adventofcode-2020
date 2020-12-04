#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace qi = boost::spirit::qi;


struct line_t
{
	int a, b;
	char c;
	std::string s;
};

BOOST_FUSION_ADAPT_STRUCT(
	line_t,
	( int, a )
	( int, b )
	( char, c )
	( std::string, s )
)

struct line_grammar_t : qi::grammar< std::string::const_iterator, line_t() >
{
	line_grammar_t()
		: line_grammar_t::base_type( start )
	{
		using qi::char_;
		using qi::int_;

		start %= int_ >> '-' >> int_ >> ' ' >> char_( 'a', 'z' ) >> ": " >> +char_( 'a', 'z' ); 
	}

	qi::rule< std::string::const_iterator, line_t() > start;
};

std::istream & operator>>( std::istream & is, line_t & line )
{
	static line_grammar_t gr;
	std::string s;
	std::getline( is, s );

	std::string::const_iterator b{ s.begin() }, e{ s.end() };
	line.s.clear();
	if( ( b != e ) && !qi::phrase_parse( b, e, gr, qi::space, line ) )
		throw std::runtime_error( "bad input for line_t" );

	return is;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	size_t const count(
		std::count_if(
			std::istream_iterator< line_t >( ifs ), std::istream_iterator< line_t >(),
			[]( line_t const & d ) -> bool
			{
				size_t score( 0 );	   
				if( d.s[ d.a - 1 ] == d.c )
					++score;
				if( d.s[ d.b - 1 ] == d.c )
					++score;
				return score == 1;
			}	
		)
	);

	std::cout << "count = " << count << std::endl;

	return 0;
}