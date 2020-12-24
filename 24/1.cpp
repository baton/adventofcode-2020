#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include <numeric>


typedef std::array< int, 3 > pos_t;


int main( int argc, char * argv[] )
{
	if( argc < 2 )
		return 0;

	std::ifstream ifs( argv[ 1 ] );

	std::map< pos_t, size_t > tiles;

	for( std::string l; std::getline( ifs, l ); )
	{
		pos_t d{ 0, 0, 0 };

		std::string::const_iterator b{ l.begin() }, e{ l.end() };
		bool n( false ), s( false );
		while( b != e )
		{
			switch( *b )
			{
				case 'e':
					if( n )
						++d[ 1 ];
					else if( s )
						++d[ 2 ];
					else
						++d[ 0 ];
					n = s = false;
					break;
				case 'w':
					if( n )
						--d[ 2 ];
					else if( s )
						--d[ 1 ];
					else
						--d[ 0 ];
					n = s = false;
					break;
				case 'n':
					n = true;
					break;
				case 's':
					s = true;
					break;
			}
			++b;
		}

		pos_t dd{ 0, 0, 0 };
		while( d != dd )
		{
			dd = d;
			while( ( d[ 1 ] > 0 ) && ( d[ 2 ] > 0 ) )
			{
				++d[ 0 ];
				--d[ 1 ];
				--d[ 2 ];
			}
			while( ( d[ 1 ] < 0 ) && ( d[ 2 ] < 0 ) )
			{
				--d[ 0 ];
				++d[ 1 ];
				++d[ 2 ];
			}
			while( ( d[ 0 ] > 0 ) && ( d[ 1 ] < 0 ) )
			{
				--d[ 0 ];
				++d[ 1 ];
				++d[ 2 ];
			}
			while( ( d[ 0 ] < 0 ) && ( d[ 1 ] > 0 ) )
			{
				++d[ 0 ];
				--d[ 1 ];
				--d[ 2 ];
			}
			while( ( d[ 0 ] > 0 ) && ( d[ 2 ] < 0 ) )
			{
				--d[ 0 ];
				++d[ 1 ];
				++d[ 2 ];
			}
			while( ( d[ 0 ] < 0 ) && ( d[ 2 ] > 0 ) )
			{
				++d[ 0 ];
				--d[ 1 ];
				--d[ 2 ];
			}
		}

		++tiles[ d ];
	}

	size_t const count(
		std::count_if( tiles.begin(), tiles.end(), 
			[]( std::map< pos_t, size_t >::value_type const & t ) -> bool
			{
				return ( t.second % 2 ) != 0;
			}
		)
	);

	std::cout << "count = " << count << std::endl;

	return 0;
}