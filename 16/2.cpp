#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
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


bool matches_rule( rule_t const & r, size_t const v )
{
	return ( ( v >= r.first.first ) && ( v <= r.first.second ) )
		|| ( ( v >= r.second.first ) && ( v <= r.second.second ) );
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

	ticket_t your_ticket;
	ifs >> your_ticket;
	
	std::getline( ifs, std::string() ); // empty line
	std::getline( ifs, std::string() ); // "nearby tickets:"

	std::vector< ticket_t > tickets;
	std::remove_copy_if( std::istream_iterator< ticket_t >( ifs ), std::istream_iterator< ticket_t >(),
		std::back_inserter< std::vector< ticket_t > >( tickets ),
		[ &rules ]( ticket_t const & t ) -> bool
		{
			return !std::transform_reduce( t.begin(), t.end(),
				true,
				std::logical_and(),
				[ &rules ]( size_t const v ) -> bool
				{
					return std::find_if( rules.begin(), rules.end(),
						[ &v ]( rule_t const & r ) -> bool
						{
							return matches_rule( r, v );
						}
					) != rules.end();
				}
			);
		}
	);

	std::set< size_t > field;
	for( size_t i( 0 ); i != rules.size(); ++i )
		field.insert( i );
	typedef std::vector< std::set< size_t > > fields_t;
	fields_t fields;
	for( size_t i( 0 ); i != rules.size(); ++i )
		fields.push_back( field );

	std::vector< std::set< size_t > > valid_fields{
		std::accumulate(
			tickets.begin(), tickets.end(),
			fields,
			[ &rules ]( fields_t & f, ticket_t const & t ) -> fields_t
			{
				for( size_t i( 0 ); i != t.size(); ++i )
				{
					std::set< size_t > s;
					for( size_t j( 0 ); j != rules.size(); ++j )
						if( matches_rule( rules[ j ], t[ i ] ) )
							s.insert( j );
					
					for( std::set< size_t >::const_iterator k( f[ i ].begin() ); k != f[ i ].end(); )
						if( std::find( s.begin(), s.end(), *k ) == s.end() )
							k = f[ i ].erase( k );
						else
							++k;
				}
				return f;
			}
		)
	};

	auto is_one_sized_set{ []( std::set< size_t > const & s ) -> bool { return s.size() == 1; } };
	while( std::find_if_not( valid_fields.begin(), valid_fields.end(), is_one_sized_set ) != valid_fields.end() )
	{
		for( size_t i( 0 ); i != valid_fields.size(); ++i )
		{
			if( is_one_sized_set( valid_fields[ i ] ) )
			{
				size_t const v( *valid_fields[ i ].begin() );
				for( size_t j( 0 ); j != valid_fields.size(); ++j )
				{
					if( ( i != j ) && !is_one_sized_set( valid_fields[ j ] ) )
					{
						valid_fields[ j ].erase( v );
					}
				}
			}
		}
	}

	uint64_t const result{
		std::accumulate( valid_fields.begin(), valid_fields.end(),
			std::pair< size_t, uint64_t >( 0, 1ULL ),
			[ &your_ticket ]( std::pair< size_t, uint64_t > & r, std::set< size_t > const & f ) -> std::pair< size_t, uint64_t >
			{
				return std::pair< size_t, uint64_t >(
					r.first + 1,
					r.second * ( *f.begin() < 6 ? your_ticket[ r.first ] : 1ULL )
				);
			}
		).second
	};

	std::cout << "result = " << result << std::endl;

	return 0;
}