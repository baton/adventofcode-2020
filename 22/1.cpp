#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <algorithm>
#include <numeric>


void read( std::istream & is, std::list< size_t > & v )
{
	std::string s;
	std::getline( is, s );
	for( ; std::getline( is, s ) && !s.empty(); )
		v.push_back( std::stoi( s ) );
}


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::list< size_t > a, b;
	read( ifs, a );
	read( ifs, b );

	while( !b.empty() )
	{
		if( a.front() < b.front() )
			a.swap( b );
		a.push_back( a.front() );
		a.push_back( b.front() );
		a.pop_front();
		b.pop_front();
	}

	uint64_t const score(
		std::accumulate( a.crbegin(), a.crend(),
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