#include "test_registrator.h"
#include <time_tracker.h>

#include <boost/thread.hpp>

using namespace system_utilities::common;
using namespace std::chrono;

namespace system_utilities
{
	namespace tests_
	{
		namespace common
		{
			void time_tracker_constructor_tests()
			{
				BOOST_CHECK_NO_THROW( time_tracker< milliseconds > tt );
				time_tracker< milliseconds > tt;
				BOOST_CHECK_NO_THROW( tt.reset() );
			}
			void time_tracker_milliseconds_tests()
			{
				{
					time_tracker< milliseconds > tt;
					BOOST_CHECK_EQUAL( tt.elapsed(), static_cast< size_t >( 0 ) );
				}
				{
					static const size_t time_to_sleep = 1;
					time_tracker< milliseconds > tt;
					long long old_time = tt.elapsed();
					for ( size_t i = 0 ; i < 5 ; ++i )
					{
						boost::this_thread::sleep( boost::posix_time::milliseconds( time_to_sleep ) );
						BOOST_CHECK_EQUAL( tt.elapsed() > old_time, true );
						old_time = tt.elapsed();
					}
				}
			}
			void time_tracker_seconds_tests()
			{
				{
					time_tracker< seconds > tt;
					BOOST_CHECK_EQUAL( tt.elapsed(), static_cast< size_t >( 0 ) );
				}
				{
					time_tracker< seconds > tt;
					for ( size_t i = 0 ; i < 3 ; ++i )
					{
						boost::this_thread::sleep( boost::posix_time::milliseconds( 1001 ) );
						BOOST_CHECK_EQUAL( tt.elapsed() >= (i + 1), true );
						BOOST_CHECK_EQUAL( tt.elapsed() < (i + 1) + 2, true );
					}
				}
			}
		}
	}
}
