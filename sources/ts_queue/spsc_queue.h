#ifndef _SYSTEM_UTILITIES_COMMON_SPSC_QUEUE_H_
#define _SYSTEM_UTILITIES_COMMON_SPSC_QUEUE_H_

#include <atomic>

#include <boost/lockfree/spsc_queue.hpp>

namespace system_utilities
{
	// spsc_queue: thread safe single producer single consumer queue
	// non virtual destructor, please inherit only if you know what are you doing

    namespace common
    {
		template< class T, const size_t size = 1024 >
		class spsc_queue
		{
			typedef T* element_ptr;

			typedef boost::lockfree::spsc_queue< element_ptr, boost::lockfree::capacity< size > > queue;			

			explicit spsc_queue( const spsc_queue& );
			spsc_queue& operator=(const spsc_queue&);
		public:
			typedef typename queue::value_type value_type;
			typedef typename queue::size_type size_type;

		private:
			queue queue_;
			
			std::atomic_bool stopping_;

		public:
			explicit spsc_queue()
			{
				stopping_ = false;
			}
			// restart method: stop queue from processing, clead queue (with deleting not processed elements by delete)
            void restart()
            {
                stop_processing();
                stopping_ = false;
            }
			// stop method: stop queue, notify wait() and ts_pop() methods that wait for messages or result of processing
			// this method is thread safe
			void stop()
			{
				stopping_ = true;
			}
			// stop_processing method: stop processing method stop queue, notify wait() and ts_pop() methods and flush not poped messages with delete.
			// this method is thread safe
			void stop_processing()
			{
				stopping_ = true;
				value_type value;
				while ( queue_.pop( value ) )
					delete value;
			}
			// non virtual destructor
			// this method is thread safe
			~spsc_queue()
			{
				stop_processing();
			}
			// wait method: wait while user call stop(), stop_processing(), ~destructor() methods OR all messages will be poped out queue 
			// if stop(), stop_processing() method was called before - returns immediatly
			// this method is thread safe
            void wait()
            {
				while ( queue_.empty() && !stopping_ )
					;
            }
			// push() method: push message into queue
			// if stop(), stop_processing() method was called before - returns immediatly
			// returns true - if message was added to queue
			// returns false - if message was not added to queue, check this parameter it could be reason of memory leak
			// this method is thread safe
			bool push( value_type val )
			{
				if (stopping_)
					return false;
				while ( !queue_.push( val ) )
					;
				return true;
			}
			// pop() message returns pointer to message that was in queue
			// returns pointer to message or NULL
			// if queue is empty - returns NULL
			// it does not wait for push - just return NULL if there is no messages into queue
			// this method is thread safe
			value_type pop()
			{				
				if (queue_.empty())
					return NULL;
				value_type value;
				if ( !queue_.pop( value ) )
					return NULL;
				return value;
			}
			// ts_pop() message returns pointer to message that was in queue
			// returns pointer to message or NULL
			// if queue is empty - returns NULL
			// it does not wait for push - just return NULL if there is no messages into queue
			// this method is not thread safe!
			value_type ts_pop()
			{
				if (stopping_)
					return NULL;
				if ( queue_.empty() )
					return NULL;
				value_type value;
				if ( !queue_.pop( value ) )
					return NULL;
				return value;
			}
			// wait_pop() message returns pointer to message that was in queue
			// returns pointer to message or NULL
			// if queue is empty, wait until stop(), stop_processing(), push() will be called.
			// if queue is stopping - return NULL
			// this method wait for push
			// this method is not thread safe!
			value_type wait_pop()
			{
				if ( stopping_ )
					return NULL;
				value_type value = NULL;
				while ( !queue_.pop( value ) && !stopping_ )
					;
				return value;
			}
			// size() method: returns 0 if queue is going to stop
			size_t size() const 
			{
				if (stopping_)
					return 0;
				return queue_.read_available();
			}
			// ts_size() method: returns queue size
			// thread safe method
			size_t ts_size() const
			{
				return queue_.read_available();
			}
			// empty() method: return true if queue is going to stop
			// returns false is queue.size() > 0
			bool empty()
			{
				if (stopping_)
					return true;
				return queue_.empty();
			}
		};
	}
}


#endif // _SYSTEM_UTILITIES_COMMON_SPSC_QUEUE_H_
