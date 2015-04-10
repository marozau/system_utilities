#ifndef _SYSTEM_UTILITIES_WINCQ_QUEUE_H_
#define _SYSTEM_UTILITIES_WINCQ_QUEUE_H_

#include <atomic>

#include <Windows.h>
#include <concurrent_queue.h>

namespace system_utilities
{
	namespace common
	{
		template< class T >
		class wincq_queue
		{
			typedef T* element_ptr;

			typedef Concurrency::concurrent_queue< element_ptr > queue;

			explicit wincq_queue( const wincq_queue& );
			wincq_queue& operator=(const wincq_queue&);

		public:
			typedef typename queue::allocator_type allocator_type;
			typedef typename queue::value_type value_type;
			typedef typename queue::size_type size_type;
			typedef typename queue::reference reference;
			typedef typename queue::const_reference const_reference;

		private:
			queue queue_;

			const size_t size_;
			std::atomic_bool stopping_;

		public:
			explicit wincq_queue( const size_t size = 128 )
				: size_( size )
			{
				stopping_.store( false, std::memory_order_release );
			}
			// restart method: stop queue from processing, clead queue (with deleting not processed elements by delete)
			void restart()
			{
				stop_processing();
				stopping_.store( false, std::memory_order_release );
			}
			// stop method: stop queue, notify wait() and ts_pop() methods that wait for messages or result of processing
			// this method is thread safe
			void stop()
			{
				stopping_.store( true, std::memory_order_release );
			}
			// stop_processing method: stop processing method stop queue, notify wait() and ts_pop() methods and flush not poped messages with delete.
			// this method is thread safe
			void stop_processing()
			{
				stopping_.store( true, std::memory_order_release );

				value_type value = NULL;
				while ( queue_.try_pop( value ) )
					delete value;
			}
			// non virtual destructor
			// this method is thread safe
			~wincq_queue()
			{
				stop_processing();
			}

			// push() method: push message into queue
			// if stop(), stop_processing() method was called before - returns immediatly
			// returns true - if message was added to queue
			// returns false - if message was not added to queue, check this parameter it could be reason of memory leak
			// this method is thread safe
			bool push( value_type val )
			{
				if ( stopping_.load( std::memory_order_consume ) )
					return false;
				queue_.push( val );
				return true;
			}

			bool try_push( value_type val )
			{
				if ( stopping_.load( std::memory_order_consume ) )
					return false;
				if ( queue_.unsafe_size() >= size_ )
					return false;
				queue_.push( val );
				return true;
			}
			// pop() message returns pointer to message that was in queue
			// returns pointer to message or NULL
			// if queue is empty - returns NULL
			// it does not wait for push - just return NULL if there is no messages into queue
			// this method is thread safe
			value_type pop()
			{
				if ( stopping_.load( std::memory_order_consume ) )
					return false;
				value_type result = NULL;
				queue_.try_pop( result );
				return result;
			}

			bool try_pop( value_type& value )
			{
				if ( stopping_.load( std::memory_order_consume ) )
					return false;
				return queue_.try_pop( value );
			}
			
			// wait_pop() message returns pointer to message that was in queue
			// returns pointer to message or NULL
			// if queue is empty, wait until stop(), stop_processing(), push() will be called.
			// if queue is stopping - return NULL
			// this method wait for push
			// this method is not thread safe!
			value_type wait_pop()
			{
				if ( stopping_.load( std::memory_order_consume ) )
					return NULL;
				value_type result = NULL;
				while ( !queue_.try_pop( result ) && !stopping_.load( std::memory_order_consume ) )
					Sleep( 1 );
				return result;
			}
			
			// ts_size() method: returns queue size
			// thread safe method
			size_t size() const
			{
				return queue_.unsafe_size();
			}
			// empty() method: return true if queue is going to stop
			// returns false is queue.size() > 0
			bool empty() const
			{
				if ( stopping_.load( std::memory_order_consume ) )
					return true;
				return queue_.empty();
			}
		};
	}
}


#endif // _SYSTEM_UTILITIES_WINCQ_QUEUE_H_
