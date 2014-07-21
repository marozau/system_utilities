#include "time_tracker.h"

namespace system_utilities
{
	namespace common
	{
		time_tracker::time_tracker()
			: start_( std::chrono::high_resolution_clock::now( ) )
		{
		}
		time_tracker::time_tracker(const time_tracker& other)
			: start_( other.start_ )
		{
		}
		time_tracker::~time_tracker()
		{
		}
		//
		void time_tracker::reset()
		{
			boost::mutex::scoped_lock lock( protect_start_ );
			start_ = std::chrono::high_resolution_clock::now( );
		}
		long long time_tracker::nanoseconds() const
		{
			boost::mutex::scoped_lock lock( protect_start_ );

			auto elapsed = std::chrono::high_resolution_clock::now( ) - start_;
			const auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count( );
			return time;
		}
		long long time_tracker::microseconds( ) const
		{
			boost::mutex::scoped_lock lock( protect_start_ );

			auto elapsed = std::chrono::high_resolution_clock::now( ) - start_;
			const auto time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count( );
			return time;
		}
		long long time_tracker::milliseconds() const
		{
			boost::mutex::scoped_lock lock( protect_start_ );

			auto elapsed = std::chrono::high_resolution_clock::now( ) - start_;
			const auto time = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count( );
			return time;
		}
		long long time_tracker::seconds() const
		{
			boost::mutex::scoped_lock lock( protect_start_ );
			auto elapsed = std::chrono::high_resolution_clock::now( ) - start_;
			const auto time = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count( );
			return time;
		}
	}
}