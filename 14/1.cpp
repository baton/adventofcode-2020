#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>


struct line_t : std::string
{};

std::istream & operator>>( std::istream & is, line_t & line )
{
	return std::getline( is, line );
}


struct machine_t
{
	typedef std::map< uint64_t, uint64_t > mem_t;
	mem_t mem;
	std::pair< uint64_t, uint64_t > mask = { std::numeric_limits< uint64_t >::max(), 0 };

	inline uint64_t apply_mask( uint64_t const val )
	{
		return ( val | mask.first ) & mask.second;
	}

	void exec( std::string const & s )
	{
		static size_t const mask_eq_len{ strlen( "mask = " ) };

		if( s.substr( 0, 4 ) == "mask" )
		{
			mask = std::accumulate( s.begin() + mask_eq_len, s.end(),
				std::pair< uint64_t, uint64_t >( 0, 0 ),
				[]( std::pair< uint64_t, uint64_t > & m, char const c ) -> std::pair< uint64_t, uint64_t >
				{
					m.first = ( m.first << 1 ) | ( c == '1' ? 1 : 0 );
					m.second = ( m.second << 1 ) | ( c == '0' ? 0 : 1 );
					return m;
				}
			);
		}
		else if( s.substr( 0, 3 ) == "mem" )
		{
			uint64_t const addr( std::atoll( s.c_str() + s.find( '[' ) + 1 ) );
			uint64_t const value( std::atoll( s.c_str() + s.find( ' ' ) + 2 ) );

			mem[ addr ] = apply_mask( value );
		}
	}
};


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	machine_t machine;
	std::for_each( std::istream_iterator< line_t >( ifs ), std::istream_iterator< line_t >(),
		[ &machine ]( std::string const & s ) -> void
		{
			machine.exec( s );
		}
	);

	uint64_t const result(
		std::transform_reduce( machine.mem.begin(), machine.mem.end(),
			0LL, std::plus(),
			[]( machine_t::mem_t::value_type const & v )
			{
				return v.second;
			}
		)
	);

	std::cout << "result = " << result << std::endl;

	return 0;
}