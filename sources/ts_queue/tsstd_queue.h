#ifndef _SYSTEM_UTILITIES_COMMON_TSSTD_QUEUE_H_
#define _SYSTEM_UTILITIES_COMMON_TSSTD_QUEUE_H_

#include <deque>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace system_utilities
{
	namespace common
	{
		template<
			class T,
				template< typename, typename > class container = std::deque,
				template< typename > class real_allocator_type = std::allocator >
		class tsstd_queue
		{
			class iterator;
			friend class iterator;

			typedef T* element_ptr;

			typedef container< element_ptr, real_allocator_type< element_ptr > > queue;

			explicit tsstd_queue( const tsstd_queue& );
			tsstd_queue& operator=(const tsstd_queue&);
		public:
			typedef typename queue::allocator_type allocator_type;
			typedef typename queue::value_type value_type;
			typedef typename queue::size_type size_type;
			typedef typename queue::reference reference;
			typedef typename queue::const_reference const_reference;

		private:
			const size_t size_;
			queue queue_;

			mutable std::mutex queue_protector_;
			std::condition_variable push_;
			std::condition_variable wait_;

			std::atomic_bool stopping_;

		public:
			explicit tsstd_queue( const size_t size = 128 )
				: size_( size )
				, queue_()				
			{
				stopping_.store( false, std::memory_order::memory_order_release );
			}
			// restart method: stop queue from processing, clead queue (with deleting not processed elements by delete)
			void restart()
			{
				stop_processing();
				stopping_.store( false, std::memory_order::memory_order_release );
			}
			// stop method: stop queue, notify wait() and ts_pop() methods that wait for messages or result of processing
			// this method is thread safe
			void stop()
			{
				stopping_.store( true, std::memory_order::memory_order_release );
				std::unique_lock<std::mutex> lock( queue_protector_ );
				push_.notify_all();
				wait_.notify_all();
			}
			// stop_processing method: stop processing method stop queue, notify wait() and ts_pop() methods and flush not poped messages with delete.
			// this method is thread safe
			void stop_processing()
			{
				stopping_.store( true, std::memory_order::memory_order_release );
				std::unique_lock<std::mutex> lock( queue_protector_ );
				while ( !queue_.empty() )
				{
					delete queue_.front();
					queue_.pop_front();
				}
				push_.notify_all();
				wait_.notify_all();
			}
			// non virtual destructor
			// this method is thread safe
			~tsstd_queue()
			{
				stop_processing();
			}
			// wait method: wait while user call stop(), stop_processing(), ~destructor() methods OR all messages will be poped out queue 
			// if stop(), stop_processing() method was called before - returns immediatly
			// this method is thread safe
			void wait()
			{
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return;
				std::unique_lock<std::mutex> lock( queue_protector_ );
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return;
				while ( !queue_.empty() && !stopping_ )
					wait_.wait( lock );
			}
			// push() method: push message into queue
			// if stop(), stop_processing() method was called before - returns immediatly
			// returns true - if message was added to queue
			// returns false - if message was not added to queue, check this parameter it could be reason of memory leak
			// this method is thread safe
			bool push( value_type val )
			{
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return false;
				std::unique_lock<std::mutex> lock( queue_protector_ );
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return false;
				queue_.push_back( val );
				lock.unlock();
				push_.notify_one();
				return true;
			}

			// push() method: push message into queue
			// if stop(), stop_processing() method was called before - returns immediatly
			// returns true - if message was added to queue
			// returns false - if message was not added to queue, check this parameter it could be reason of memory leak
			// this method is thread safe
			bool try_push( value_type val )
			{
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return false;
				std::unique_lock<std::mutex> lock( queue_protector_ );
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return false;
				if ( queue_.size() >= size_ )
					return false;
				queue_.push_back( val );
				lock.unlock();
				push_.notify_one();
				return true;
			}
			// pop() message returns pointer to message that was in queue
			// returns pointer to message or NULL
			// if queue is empty - returns NULL
			// it does not wait for push - just return NULL if there is no messages into queue
			// this method is thread safe
			value_type pop()
			{
				if ( queue_.empty() )
					return NULL;
				value_type result = queue_.front();
				queue_.pop_front();
				if ( queue_.empty() )
				{
					std::unique_lock<std::mutex> lock( queue_protector_ );
					wait_.notify_all();
				}
				return result;
			}
			// ts_pop() message returns pointer to message that was in queue
			// returns pointer to message or NULL
			// if queue is empty - returns NULL
			// it does not wait for push - just return NULL if there is no messages into queue
			// this method is not thread safe!
			value_type ts_pop()
			{
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return NULL;
				std::unique_lock<std::mutex> lock( queue_protector_ );
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return NULL;
				if ( queue_.empty() )
					return NULL;
				value_type result = queue_.front();
				queue_.pop_front();
				if ( queue_.empty() )
					wait_.notify_all();
				return result;
			}
			// wait_pop() message returns pointer to message that was in queue
			// returns pointer to message or NULL
			// if queue is empty, wait until stop(), stop_processing(), push() will be called.
			// if queue is stopping - return NULL
			// this method wait for push
			// this method is not thread safe!
			value_type wait_pop()
			{
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return NULL;
				std::unique_lock<std::mutex> lock( queue_protector_ );
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return NULL;
				while ( queue_.empty() )
				{
					push_.wait( lock );
					if ( stopping_.load( std::memory_order::memory_order_consume ) )
						return NULL;
				}
				value_type result = queue_.front();
				queue_.pop_front();
				if ( queue_.empty() )
					wait_.notify_all();
				return result;
			}
			// size() method: returns 0 if queue is going to stop
			size_t size() const
			{
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return 0;
				std::unique_lock<std::mutex> lock( queue_protector_ );
				return queue_.size();
			}
			// ts_size() method: returns queue size
			// thread safe method
			size_t ts_size() const
			{
				std::unique_lock<std::mutex> lock( queue_protector_ );
				return queue_.size();
			}
			// empty() method: return true if queue is going to stop
			// returns false is queue.size() > 0
			bool empty() const
			{
				if ( stopping_.load( std::memory_order::memory_order_consume ) )
					return true;
				std::unique_lock<std::mutex> lock( queue_protector_ );
				return queue_.empty();
			}
		};
	}
}


#endif // _SYSTEM_UTILITIES_COMMON_TSSTD_QUEUE_H_
