#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <algorithm>
#include <numeric>


typedef std::pair< size_t, std::list< size_t > > player_t;


void read( std::istream & is, std::list< size_t > & v )
{
	std::string s;
	std::getline( is, s );
	for( ; std::getline( is, s ) && !s.empty(); )
		v.push_back( std::stoi( s ) );
}

bool game( player_t & a, player_t & b )
{
	size_t const pp( a.first );
	std::list< std::pair< player_t, player_t > > rounds;
	while( !b.second.empty() )
	{
		for( std::pair< player_t, player_t > const & r : rounds )
		{
			if( ( ( a == r.first ) && ( b == r.second ) )
				|| ( b == r.first ) && ( a == r.second ) )
			{
				return pp != 1;
			}
		}

		rounds.push_back( { a, b } );

		if( ( ( a.second.size() - 1 ) >= a.second.front() )
			&& ( ( b.second.size() - 1 ) >= b.second.front() ) )
		{
			std::list< size_t > na( std::next( a.second.begin() ), std::next( a.second.begin(), a.second.front() + 1 ) );
			std::list< size_t > nb( std::next( b.second.begin() ), std::next( b.second.begin(), b.second.front() + 1 ) );

			if( game( player_t( a.first, na ), player_t( b.first, nb ) ) )
				a.swap( b );
		}
		else if( a.second.front() < b.second.front() )
			a.swap( b );

		a.second.push_back( a.second.front() );
		a.second.push_back( b.second.front() );
		a.second.pop_front();
		b.second.pop_front();
	}

	return a.first != pp;
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	player_t a{ 1, {} }, b{ 2, {} };
	read( ifs, a.second );
	read( ifs, b.second );

	game( a, b );
	if( a.second.empty() )
		a.swap( b );

	uint64_t const score(
		std::accumulate( a.second.crbegin(), a.second.crend(),
			std::pair < uint64_t, uint64_t >( 0ULL, 0ULL ),
			[]( std::pair< uint64_t, uint64_t > & i, size_t const x ) -> std::pair< uint64_t, uint64_t >
			{
				i.second += ++i.first * x;
				return std::move( i );
			}
		).second
	);

	std::cout << "score = " << score << std::endl;

	return 0;
}