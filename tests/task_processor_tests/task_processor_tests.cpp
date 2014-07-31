#include "test_registrator.h"

#include <vector>

#include <task_processor.h>

#include <time_tracker.h>

using namespace system_utilities::common;

namespace system_utilities
{
	namespace tests_
	{
		namespace details
		{
			struct counter
			{
				mutable boost::mutex protect_count_;
				volatile size_t count_;
			public:
				explicit counter()
					: count_( 0 )
				{
				}
				void increment()
				{
					boost::mutex::scoped_lock lock( protect_count_ );
					for(int i = 0 ; i < 400 ; ++i )
					{
						int a = 1000 % (1+i) + 45;
						a = a * 65 & 34;
						if ( a % 45 == 44 )
						{
							++count_;
						}
						if ( a % 45 == 44 )
						{
							--count_;
						}
					}
					++count_;
				}
				size_t count() const
				{
					boost::mutex::scoped_lock lock( protect_count_ );
					return count_;
				}
			};
			class task
			{
				counter& counter_;
			public:
				explicit task( counter& c)
					: counter_( c )
				{
				}
				void operator()()
				{
					counter_.increment();
				}
			};
			struct my_allocator
			{
				typedef std::vector< counter* > counters;
				counters counters_;
				typedef std::vector< task* > tasks;
				tasks tasks_;
				size_t index_;
				explicit my_allocator()
				{
				}
				explicit my_allocator( size_t counter_size )
					: index_( 0 )
				{
					for( size_t i = 0 ; i < counter_size ; ++i )
					{
						counters_.push_back( new counter() );
						tasks_.push_back( new task( *(counters_[ i ]) ) );
					}
				}
				~my_allocator()
				{
					for( counters::const_iterator i = counters_.begin() ; i != counters_.end() ; ++i )
						delete *i;
					for( tasks::const_iterator i = tasks_.begin() ; i != tasks_.end() ; ++i )
						delete *i;
				}
				task* allocate()
				{
					if ( index_ == tasks_.size() )
						index_ = 0;
					return tasks_[ index_++ ];
				}
				void destroy( task* )
				{
				}
				void deallocate( task*, size_t )
				{
				}
				size_t count() const
				{
					size_t result = 0;
					for( counters::const_iterator i = counters_.begin() ; i != counters_.end() ; ++i )
						result += (*i)->count();
					return result;
				}
			};

		}
		namespace common
		{
			void task_processor_constructor_tests()
			{
				BOOST_CHECK_NO_THROW( task_processor< details::task > ts( 4 ) );
			}
			void task_processor_add_task_tests()
			{
				task_processor< details::task > ts( 4 );
				details::counter c;
				BOOST_CHECK_EQUAL( c.count(), 0U );
				ts.add_task( ts.create_task( c ) );
				boost::this_thread::sleep( boost::posix_time::milliseconds( 1 ) );
				BOOST_CHECK_EQUAL( c.count(), 1U );
			}
			void task_processor_add_task_performace_tests()
			{
				time_tracker< std::chrono::milliseconds > tt;
				static const size_t thread_size = 4;
				static const size_t counter_size = 8;
				static const size_t tasks_size = 2000000;
				task_processor< details::task > ts( thread_size, true );
				details::counter counters[ counter_size ];
				for( size_t i = 0 ; i < counter_size ; ++i )
					BOOST_CHECK_EQUAL( counters[ i ].count(), 0U );
				for( size_t i = 0 ; i < tasks_size ; ++i )
					ts.add_task( ts.create_task( counters[ rand() % counter_size ] ) );
				ts.stop();
				size_t count_summ = 0;
				for( size_t i = 0 ; i < counter_size ; ++i )
				{
					count_summ += counters[ i ].count();
				}
				BOOST_CHECK_EQUAL( count_summ, tasks_size );
				BOOST_CHECK_EQUAL( tt.elapsed( ) < 2200, true );
			}
			void task_processor_wait_tests()
			{
				details::counter c;
				task_processor< details::task > tp( 1 );
				tp.add_task( tp.create_task( c ) );
				tp.add_task( tp.create_task( c ) );
				tp.add_task( tp.create_task( c ) );
				tp.add_task( tp.create_task( c ) );
				BOOST_CHECK_EQUAL( tp.size() > 0ul, true );
				tp.add_task( tp.create_task( c ) );
				BOOST_CHECK_NO_THROW( tp.wait() );
				BOOST_CHECK_EQUAL( tp.size(), 0U );
			}
			void task_processor_own_allocator_performance_tests()
			{
				time_tracker< std::chrono::milliseconds > tt;
				static const size_t thread_size = 4;
				static const size_t counter_size = 8;
				static const size_t tasks_size = 2000000;

				details::my_allocator allocator( counter_size );
				typedef task_processor< details::task, ts_queue< details::task >, details::my_allocator > own_tp;
				own_tp tp( thread_size, true );
				for( size_t i = 0 ; i < tasks_size ; ++i )
					tp.add_task( allocator.allocate() );
				tp.stop();

				BOOST_CHECK_EQUAL( allocator.count(), tasks_size );
				std::cout << tt.elapsed( ) << std::endl;
				BOOST_CHECK_EQUAL( tt.elapsed( ) < 1450, true );
			}
		}
	}
}
