#include <boost/test/test_tools.hpp>
#include <dynamic_link_helper.h>

namespace system_utilities
{
	namespace tests_
	{
		namespace common
		{
			void ts_queue_constructor_tests();
			void ts_queue_different_threads_tests();
			void ts_queue_wait_pop_tests();
			void ts_queue_many_threads_tests();
			void ts_queue_another_container_tests();
			//
			void wincq_queue_constructor_tests();
			void wincq_queue_different_threads_tests();
			void wincq_queue_wait_pop_tests();
			void wincq_queue_many_threads_tests();
			//
			void tsstd_queue_constructor_tests();
			void tsstd_queue_push_limit_tests();
			void tsstd_queue_different_threads_tests();
			void tsstd_queue_wait_pop_tests();
			void tsstd_queue_many_threads_tests();
			void tsstd_queue_another_container_tests();
			//
			void lf_queue_constructor_tests();
			void lf_queue_different_threads_tests();
			void lf_queue_wait_pop_tests();
			void lf_queue_many_threads_tests();
			//
			void spsc_queue_constructor_tests();
			void spsc_queue_different_threads_tests();
			void spsc_queue_wait_pop_tests();
			void spsc_queue_many_threads_tests();
		}
	}
}

