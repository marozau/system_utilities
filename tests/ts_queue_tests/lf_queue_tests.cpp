#include "test_registrator.h"

#include <deque>
#include <queue>

#include <lf_queue.h>
#include <time_tracker.h>

#include <boost/thread.hpp>

using namespace system_utilities::common;

namespace system_utilities
{
	namespace tests_
	{
		namespace common
		{
			namespace details
			{
				void lf_queue_constructor_test_helper()
				{
					lf_queue< size_t > mq;
					for (int i = 1 ; i < 10 ; i ++)
						mq.push( new size_t(i) );
					size_t *s = mq.pop();
					BOOST_CHECK_EQUAL( *s, (size_t)1 );
					delete s;
					for (int i = 10 ; i < 14 ; i ++)
						mq.push( new size_t(i) );
					size_t i = 2;
					while (!mq.empty())
					{
						size_t* s = mq.pop();
						BOOST_CHECK_EQUAL( *s, i++ );
						delete s;
					}
					BOOST_CHECK_EQUAL( mq.empty(), true );
				}
				void lf_queue_constructor_test_helper_with_wait_strategy()
				{
					lf_queue< size_t > mq( [](){ boost::this_thread::sleep( boost::posix_time::milliseconds( 50 ) ); } );
					for ( int i = 1; i < 10; i++ )
						mq.push( new size_t( i ) );
					size_t *s = mq.pop();
					BOOST_CHECK_EQUAL( *s, ( size_t )1 );
					delete s;
					for ( int i = 10; i < 14; i++ )
						mq.push( new size_t( i ) );
					size_t i = 2;
					while ( !mq.empty() )
					{
						size_t* s = mq.pop();
						BOOST_CHECK_EQUAL( *s, i++ );
						delete s;
					}
					BOOST_CHECK_EQUAL( mq.empty(), true );
				}
				typedef lf_queue< size_t > lf_queue_size_t;
				void lf_queue_different_threads_pop_thread_helper( lf_queue_size_t* mq, const size_t different_threads_test_size )
				{
					boost::this_thread::sleep( boost::posix_time::milliseconds( 20 ) );
					size_t i = 0;
					while ( i < different_threads_test_size )
					{
						size_t* s = mq->wait_pop();
						BOOST_CHECK_EQUAL( i, *s );
						delete s;
						i++;
					}
					BOOST_CHECK_EQUAL( i, different_threads_test_size );
					BOOST_CHECK_EQUAL( mq->empty(), true );
				}
				void lf_queue_wait_pop_test_helper( lf_queue_size_t* mq )
				{
					time_tracker< std::chrono::seconds > tt;
					using namespace boost::posix_time;
					size_t *s = mq->wait_pop();
					BOOST_CHECK_EQUAL( *s, (size_t)15 );
					delete s;
					BOOST_CHECK_EQUAL( tt.elapsed() >= 1, true );
					BOOST_CHECK_EQUAL( mq->empty(), true );
				}
				struct lf_queue_many_threads_test_helper
				{
					lf_queue_size_t mq_;
					
					explicit lf_queue_many_threads_test_helper( const size_t push_threads, const size_t pop_threads, const size_t message_size )
					{
						BOOST_CHECK_EQUAL( mq_.empty(), true );
						boost::thread_group tg_push, tg_pop;
						for (size_t i = 0 ; i < push_threads ; ++i)
							tg_push.create_thread( boost::bind( &lf_queue_many_threads_test_helper::pusher, this, message_size ) );
						const size_t total_message_size = message_size * push_threads;
						const size_t to_pop = total_message_size / pop_threads;
						for (size_t i = 0 ; i < pop_threads ; ++i)
							tg_pop.create_thread( boost::bind( &lf_queue_many_threads_test_helper::poper, this, to_pop ) );
						tg_push.join_all();						
						tg_pop.join_all();
						mq_.stop_processing();
					}
					void pusher( const size_t size )
					{
						for ( size_t i = 0; i < size; ++i )
						{
							size_t * s = new size_t( rand() % 100000 );
							mq_.push( s );							
						}
					}
					void poper( const size_t size )
					{
						for ( size_t i = 0; i < size; ++i )
						{
							size_t* s = mq_.wait_pop();
							delete s;
						}
					}
				};
				void lf_queue_wait_test_helper(details::lf_queue_size_t* mq_, size_t* pop_iterations_)
				{
					while (true)
					{
						size_t* s = mq_->wait_pop();
						if (!s)
							break;
						delete s;
						(*pop_iterations_)++;
					}
				}
			}
			void lf_queue_constructor_tests()
			{
				lf_queue< int > queue;
				details::lf_queue_constructor_test_helper();
				details::lf_queue_constructor_test_helper_with_wait_strategy();
			}
			void lf_queue_different_threads_tests()
			{
				{
					details::lf_queue_size_t mq;
					const size_t different_threads_test_size = 1000000;
					boost::thread pop = boost::thread( boost::bind( &details::lf_queue_different_threads_pop_thread_helper, &mq, different_threads_test_size ) );
					using namespace boost::posix_time;
					for ( size_t i = 0; i < different_threads_test_size; i++ )
					{
						mq.push( new size_t( i ) );
					}
					pop.join();
					BOOST_CHECK_EQUAL( mq.empty(), true );
				}
				{
					details::lf_queue_size_t mq( [](){ boost::this_thread::sleep( boost::posix_time::milliseconds( 50 ) ); } );
					const size_t different_threads_test_size = 1000000;
					boost::thread pop = boost::thread( boost::bind( &details::lf_queue_different_threads_pop_thread_helper, &mq, different_threads_test_size ) );
					using namespace boost::posix_time;
					for ( size_t i = 0; i < different_threads_test_size; i++ )
					{
						mq.push( new size_t( i ) );
					}
					pop.join();
					BOOST_CHECK_EQUAL( mq.empty(), true );
				}
			}
			void lf_queue_wait_pop_tests()
			{
				details::lf_queue_size_t mq;
				BOOST_CHECK_EQUAL( mq.empty(), true );
				boost::thread pop = boost::thread( boost::bind( &details::lf_queue_wait_pop_test_helper, &mq ) );
				boost::this_thread::sleep( boost::posix_time::milliseconds( 1200 ) );
				mq.push( new size_t(15) );
				pop.join();
				BOOST_CHECK_EQUAL( mq.empty(), true );
			}
			void lf_queue_many_threads_tests()
			{
				const size_t message_amount = 1000000;
				{
					time_tracker< std::chrono::milliseconds > tt;
					details::lf_queue_many_threads_test_helper helper( 4, 4, message_amount );
					const auto time = tt.elapsed();
					BOOST_CHECK_EQUAL( time, 100 );
				}
				{
					time_tracker< std::chrono::milliseconds > tt;
					details::lf_queue_many_threads_test_helper helper( 4, 1, message_amount );
					const auto time = tt.elapsed();
					BOOST_CHECK_EQUAL( time, 100 );
				}
			}
			void lf_queue_wait_tests()
			{
				details::lf_queue_size_t mq;
				mq.restart();
				BOOST_CHECK_EQUAL( mq.empty(), true );
				const size_t size = 1000000;
				for(size_t i = 0 ; i < size ; ++i )
				{
					size_t* s = new size_t(i);
					mq.push( s );
				}
				size_t pop_iterations_ = 0;
				boost::thread poper( boost::bind( &details::lf_queue_wait_test_helper, &mq, &pop_iterations_ ) );
				mq.wait();
				mq.stop();
				poper.join();
				BOOST_CHECK_EQUAL(pop_iterations_, size);
			}
		}
	}
}
