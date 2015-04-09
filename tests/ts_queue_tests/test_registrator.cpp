#include "test_registrator.h"

#include <boost/test/unit_test.hpp>

TEST_INIT
{
	boost::unit_test_framework::test_suite& master_test_suite = boost::unit_test::framework::master_test_suite();

	using namespace system_utilities::tests_::common;
	master_test_suite.add( BOOST_TEST_CASE( &ts_queue_constructor_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &ts_queue_different_threads_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &ts_queue_wait_pop_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &ts_queue_another_container_tests ) );

	master_test_suite.add( BOOST_TEST_CASE( &tsstd_queue_constructor_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &tsstd_queue_push_limit_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &tsstd_queue_different_threads_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &tsstd_queue_wait_pop_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &tsstd_queue_another_container_tests ) );

	master_test_suite.add( BOOST_TEST_CASE( &lf_queue_constructor_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &lf_queue_different_threads_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &lf_queue_wait_pop_tests ) );

	master_test_suite.add( BOOST_TEST_CASE( &spsc_queue_constructor_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &spsc_queue_different_threads_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &spsc_queue_wait_pop_tests ) );

#ifdef RUN_PERFORMANCE_TESTS
	master_test_suite.add( BOOST_TEST_CASE( &ts_queue_many_threads_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &tsstd_queue_many_threads_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &lf_queue_many_threads_tests ) );
	master_test_suite.add( BOOST_TEST_CASE( &spsc_queue_many_threads_tests ) );
#endif

	return TEST_RETURN;
}

DEFINE_MAIN_FOR_DYNAMIC
