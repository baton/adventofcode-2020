#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include <memory>

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/adapted/std_pair.hpp>

namespace qi = boost::spirit::qi;


typedef std::pair< std::string, std::string > key_value_t;

template< typename Iterator >
struct key_value_grammar_t : qi::grammar< Iterator, key_value_t() >
{
	key_value_grammar_t()
		: key_value_grammar_t::base_type( start )
	{
		using qi::char_;
		using qi::int_;

		start %= key >> ':' >> value;
		key = char_( 'a', 'z' ) >> char_( 'a', 'z' ) >> char_( 'a', 'z' );
		value = +( char_( 'a', 'z' ) | char_( '0', '9' ) | char_( '#' ) );
	}

	qi::rule< Iterator, key_value_t() > start;
	qi::rule< Iterator, std::string() > key, value;
};


typedef std::map< std::string, std::string > data_t;

namespace std
{
	std::istream & operator>>( std::istream & is, data_t & data )
	{
		static key_value_grammar_t< std::string::const_iterator > key_value_grammar;

		data.clear();
		std::string s;
		for( std::getline( is, s ); !s.empty(); std::getline( is, s ) )
		{
			std::string::const_iterator b{ s.begin() }, e{ s.end() };
			while( b != e )
			{
				key_value_t kv;
				if( qi::phrase_parse( b, e, key_value_grammar, qi::space, kv ) )
					data.insert( kv );
				else
					throw std::runtime_error( "bad input for key_value_t" );

				if( ( b != e ) && ( *b == ' ' ) )
					++b;
			}
			if( is.eof() )
				break;
		}

		return is;
	}
}


bool is_valid_year( std::string const & s, size_t const l, size_t const h )
{
	if( s.length() != 4 )
		return false;

	std::string::const_iterator b{ s.begin() }, e{ s.end() };
	size_t y( 0 );
	if( !qi::phrase_parse( b, e, qi::uint_, qi::space, y ) || ( b != e ) )
		return false;

	return ( l <= y ) && ( y <= h );
}


typedef std::map< std::string, std::function< bool( std::string const & ) > > validators_t;
validators_t const validators
{
	{
		"byr",
		[]( std::string const & s ) -> bool
		{
			return is_valid_year( s, 1920, 2002 );
		}
	},
	{
		"iyr",
		[]( std::string const & s ) -> bool
		{
			return is_valid_year( s, 2010, 2020 );
		}
	},
	{
		"eyr",
		[]( std::string const & s ) -> bool
		{
			return is_valid_year( s, 2020, 2030 );
		}
	},
	{
		"hgt",
		[]( std::string const & s ) -> bool
		{
			std::pair< size_t, std::string > val;

			std::string::const_iterator b{ s.begin() }, e{ s.end() };
			if( !qi::phrase_parse( b, e, qi::uint_ >> +qi::char_, qi::space, val ) || ( b != e ) )
				return false;

			if( val.second == "cm" )
				return ( val.first >= 150 ) && ( val.first <= 193 );
			if( val.second == "in" )
				return ( val.first >= 59 ) && ( val.first <= 76 );
			return false;
		}
	},
	{
		"hcl",
		[]( std::string const & s ) -> bool
		{
			std::string c;
			std::string::const_iterator b{ s.begin() }, e{ s.end() };
			if( !qi::phrase_parse( b, e, qi::lit( '#' ) >> +( qi::char_( '0', '9' ) | qi::char_( 'a', 'f' ) ), qi::space, c ) || ( b != e ) )
				return false;

			return c.length() == 6;
		}
	},
	{
		"ecl",
		[]( std::string const & s ) -> bool
		{
			return ( s == "amb" )
				|| ( s == "blu" )
				|| ( s == "brn" )
				|| ( s == "gry" )
				|| ( s == "grn" )
				|| ( s == "hzl" )
				|| ( s == "oth" );
		}
	},
	{
		"pid",
		[]( std::string const & s ) -> bool
		{
			std::string::const_iterator b{ s.begin() }, e{ s.end() };
			if( !qi::phrase_parse( b, e, +qi::char_( '0', '9' ), qi::space ) || ( b != e ) )
				return false;

			return s.length() == 9;
		}
	}
};


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	size_t const count(
		std::count_if(
			std::istream_iterator< data_t >( ifs ), std::istream_iterator< data_t >(),
			[]( data_t const & data ) -> bool
			{
				return std::accumulate( validators.begin(), validators.end(),
					true,
					[ &data ]( bool b, validators_t::value_type const & v ) -> bool
					{
						data_t::const_iterator i( data.find( v.first ) );
						return b && ( i != data.end() ) && v.second( i->second );
					}
				);
			}
		)
	);

	std::cout << "count = " << count << std::endl;

	return 0;
}