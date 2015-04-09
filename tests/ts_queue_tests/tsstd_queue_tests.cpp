#include "test_registrator.h"

#include <deque>
#include <queue>

#include <tsstd_queue.h>
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
				void tsstd_queue_constructor_test_helper()
				{
					tsstd_queue< size_t > mq;
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
				typedef tsstd_queue< size_t > tsstd_queue_size_t;
				void tsstd_queue_different_threads_pop_thread_helper( tsstd_queue_size_t* mq, const size_t different_threads_test_size )
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
					BOOST_CHECK_EQUAL( mq->size(), (size_t)0 );
				}
				void tsstd_queue_wait_pop_test_helper( tsstd_queue_size_t* mq )
				{
					time_tracker< std::chrono::seconds > tt;
					using namespace boost::posix_time;
					size_t *s = mq->wait_pop();
					BOOST_CHECK_EQUAL( *s, (size_t)15 );
					delete s;
					BOOST_CHECK_EQUAL( tt.elapsed() >= 1, true );
					BOOST_CHECK_EQUAL( mq->empty(), true );
					BOOST_CHECK_EQUAL( mq->size(), (size_t)0 );
				}
				struct tsstd_queue_many_threads_test_helper
				{
					tsstd_queue_size_t mq_;

					tsstd_queue_many_threads_test_helper() 
						: mq_( 1024 )
					{
					}
					
					explicit tsstd_queue_many_threads_test_helper( const size_t push_threads, const size_t pop_threads, const size_t message_size )
					{
						BOOST_CHECK_EQUAL( mq_.size(), (size_t)0 );
						BOOST_CHECK_EQUAL( mq_.empty(), true );
						boost::thread_group tg_push, tg_pop;
						for (size_t i = 0 ; i < push_threads ; ++i)
							tg_push.create_thread( boost::bind( &tsstd_queue_many_threads_test_helper::pusher, this, message_size ) );
						const size_t total_message_size = message_size * push_threads;
						const size_t to_pop = total_message_size / pop_threads;
						for (size_t i = 0 ; i < pop_threads ; ++i)
							tg_pop.create_thread( boost::bind( &tsstd_queue_many_threads_test_helper::poper, this, to_pop ) );
						tg_push.join_all();						
						tg_pop.join_all();
						mq_.stop_processing();
					}
					void pusher( const size_t size )
					{
						for ( size_t i = 0; i < size; ++i )
						{
							size_t * s = new size_t( rand() % 100000 );
							while( !mq_.push( s ) );
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
				void tsstd_queue_wait_test_helper(details::tsstd_queue_size_t* mq_, size_t* pop_iterations_)
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
			void tsstd_queue_constructor_tests()
			{
				tsstd_queue< int > queue;
				details::tsstd_queue_constructor_test_helper();
			}

			void tsstd_queue_push_limit_tests()
			{
				tsstd_queue< int > queue;
				for ( int i = 0; i < 128; ++i )
					BOOST_CHECK_EQUAL( queue.push(new int(i)), true );
				int* value = new int( 129 );
				BOOST_CHECK_EQUAL( queue.push( value ), false );
				delete value;
				value = queue.wait_pop();
				BOOST_CHECK_EQUAL( *value, 0 );
				delete value;
				value = new int( 129 );
				BOOST_CHECK_EQUAL( queue.push( value ), true );
			}

			void tsstd_queue_different_threads_tests()
			{
				details::tsstd_queue_size_t mq( 1024 );
				const size_t different_threads_test_size = 1000000;
				boost::thread pop = boost::thread( boost::bind( &details::tsstd_queue_different_threads_pop_thread_helper, &mq, different_threads_test_size ) );
				using namespace boost::posix_time;
				size_t counter = 0;
				for (size_t i = 0 ; i < different_threads_test_size ; i++)
				{
					size_t* value = new size_t( i );
					while ( !mq.push( value ) );
				}
				pop.join();
				BOOST_CHECK_EQUAL( mq.empty(), true );
			}
			void tsstd_queue_wait_pop_tests()
			{
				details::tsstd_queue_size_t mq;
				BOOST_CHECK_EQUAL( mq.empty(), true );
				BOOST_CHECK_EQUAL( mq.size(), (size_t)0 );
				boost::thread pop = boost::thread( boost::bind( &details::tsstd_queue_wait_pop_test_helper, &mq ) );
				boost::this_thread::sleep( boost::posix_time::milliseconds( 1200 ) );
				mq.push( new size_t(15) );
				pop.join();
				BOOST_CHECK_EQUAL( mq.empty(), true );
				BOOST_CHECK_EQUAL( mq.size(), (size_t)0 );
			}
			void tsstd_queue_many_threads_tests()
			{
				const size_t message_amount = 1000000;
				{
					time_tracker< std::chrono::milliseconds > tt;
					details::tsstd_queue_many_threads_test_helper helper( 4, 4, message_amount );
					const auto time = tt.elapsed();
					BOOST_CHECK_EQUAL( time, 100 );
				}
				{
					time_tracker< std::chrono::milliseconds > tt;
					details::tsstd_queue_many_threads_test_helper helper( 4, 1, message_amount );
					const auto time = tt.elapsed();
					BOOST_CHECK_EQUAL( time, 100 );
				}
			}
			void tsstd_queue_wait_tests()
			{
				details::tsstd_queue_size_t mq( 1024 );
				mq.restart();
				BOOST_CHECK_EQUAL( mq.size(), (size_t)0 );
				BOOST_CHECK_EQUAL( mq.empty(), true );
				const size_t size = 1000000;
				for(size_t i = 0 ; i < size ; ++i )
				{
					size_t* s = new size_t(i);
					while( !mq.push( s ) );
				}
				BOOST_CHECK_EQUAL( mq.size(), (size_t)size );
				size_t pop_iterations_ = 0;
				boost::thread poper( boost::bind( &details::tsstd_queue_wait_test_helper, &mq, &pop_iterations_ ) );
				mq.wait();
				mq.stop();
				poper.join();
				BOOST_CHECK_EQUAL(pop_iterations_, size);
			}

			void tsstd_queue_another_container_tests()
			{
				tsstd_queue< int, std::deque > deque_ts;
				deque_ts.push( new int(5) );
				int* a = deque_ts.pop();
				BOOST_CHECK_EQUAL( *a, 5 );
				delete a;
			}
		}
	}
}
