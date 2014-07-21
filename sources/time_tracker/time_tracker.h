#ifndef _SYSTEM_UTILITIES_COMMON_TIME_TRACKER_H_
#define _SYSTEM_UTILITIES_COMMON_TIME_TRACKER_H_

#include <boost/thread/mutex.hpp>

#include <chrono>

namespace system_utilities
{
	namespace common
	{
		// time_tracker class count time until it was created or reseted.
		// could be used for performance tests, for processing time calculating
		// not a virtual destructor class

		/*auto start = std::chrono::high_resolution_clock::now( );
		
			auto elapsed = std::chrono::high_resolution_clock::now( ) - start;

		long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count( );*/

		class time_tracker
		{
			mutable boost::mutex protect_start_;
			std::chrono::high_resolution_clock::time_point start_;
			
		public:
			explicit time_tracker();
			time_tracker( const time_tracker& other );
			~time_tracker();
			//
			void reset();
			long long nanoseconds() const;
			long long microseconds( ) const;
			long long milliseconds() const;
			long long seconds() const;
		};
	}
}

#endif // _SYSTEM_UTILITIES_COMMON_TIME_TRACKER_H_
