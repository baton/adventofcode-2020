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


typedef std::pair< std::string, int > op_t;
typedef std::vector< op_t > program_t;

template< typename Iterator >
struct op_grammar_t : qi::grammar< Iterator, op_t() >
{
	op_grammar_t()
		: op_grammar_t::base_type( op )
	{
		using qi::int_;
		using qi::lit;

		op = ( qi::string( "acc" ) | qi::string( "jmp" ) | qi::string( "nop" ) ) >> lit( ' ' ) >> int_;
	}

	qi::rule< Iterator, op_t() > op;
};

namespace std
{
	std::istream & operator>>( std::istream & is, op_t & op )
	{
		static op_grammar_t< std::string::const_iterator > og;

		std::string s;
		std::getline( is, s );

		if( !s.empty() )
		{
			op = op_t();
			std::string::const_iterator b{ s.begin() }, e{ s.end() };
			if( !qi::phrase_parse( b, e, og, qi::space, op ) || ( b != e ) )
				throw std::runtime_error( "bad input for op_t" );
		}

		return is;
	}
}

int run_program( program_t p, bool & complete )
{
	int accumulator( 0 );
	size_t opp( 0 );

	while( opp < p.size() )
	{
		std::string & opc( p[ opp ].first );
		if( opc == "nop" )
		{
			++opp;
		}
		else if( opc == "jmp" )
		{
			opp += p[ opp ].second;
		}
		else if( opc == "acc" )
		{
			accumulator += p[ opp ].second;
			++opp;
		}
		else
			break;

		opc.clear();
	}

	complete = opp == p.size();
	return accumulator;
}

int try_fix_and_run( program_t const & program )
{
	for( size_t opp( 0 ); opp != program.size(); ++opp )
		if( program[ opp ].first != "acc" )
		{
			program_t p( program );
			if( program[ opp ].first == "jmp" )
				p[ opp ].first = "nop";
			else
				p[ opp ].first = "jmp";

			bool complete( false );
			int r( run_program( p, complete ) );
			if( complete )
				return r;
		}

	return 0;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	program_t const program{ std::istream_iterator< op_t >( ifs ), std::istream_iterator< op_t >() };

	int const result( try_fix_and_run( program ) );

	std::cout << "result = " << result << std::endl;

	return 0;
}