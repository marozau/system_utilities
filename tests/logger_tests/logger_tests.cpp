#include "test_registrator.h"

#include <logger.h>
#include <time_tracker.h>

#include <boost/regex.hpp>

using namespace system_utilities::common;

namespace system_utilities
{
	namespace tests_
	{
		namespace common
		{
			namespace details
			{
				template< bool turn_on, bool flush_stream, bool print_prefix >
				void logger_write_performance_test_helper( const size_t millisecs_for_stream, const size_t millisecs_for_method )
				{
					typedef logger< turn_on, flush_stream, print_prefix > logger;
					{
						std::stringstream stream;
						logger l( stream );
						time_tracker< std::chrono::milliseconds > tt;
						static const size_t test_size = 200000;
						for ( size_t i = 0 ; i < test_size ; ++i )
							l.note() << "0123456789qwertyuioplkjhgfdsazxcvbnm";
						std::cout << tt.elapsed( ) << std::endl;
						BOOST_CHECK_EQUAL( tt.elapsed( ) < millisecs_for_stream, true );
					}
					{
						std::stringstream stream;
						logger l( stream );
						time_tracker< std::chrono::milliseconds > tt;
						static const size_t test_size = 200000;
						for ( size_t i = 0 ; i < test_size ; ++i )
							l.note( "0123456789qwertyuioplkjhgfdsazxcvbnm" );
						std::cout << tt.elapsed( ) << std::endl;
						BOOST_CHECK_EQUAL( tt.elapsed( ) < millisecs_for_method, true );
					}
				}
			}
			void logger_constructor_tests()
			{
				std::stringstream str;
				BOOST_CHECK_NO_THROW( (logger< false >( str )) );
				BOOST_CHECK_NO_THROW( (logger< true >( str )) );
				BOOST_CHECK_NO_THROW( (logger< true, false >( str )) );
				BOOST_CHECK_NO_THROW( (logger< true, false, false >( str )) );
				BOOST_CHECK_NO_THROW( (logger< true, true, false >( str )) );
			}
			void logger_write_tests()
			{
				{
					std::stringstream stream;
					logger< false > no_logger( stream );
					no_logger.write( system_utilities::common::details::message_level::note, "message" );
					const std::string stream_content = stream.str();
					BOOST_CHECK_EQUAL( "", stream_content );
				}
				{
					std::stringstream stream;
					logger<> log( stream );
					log.write( system_utilities::common::details::message_level::warn, "message" );
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:[A-Z ]{7}\\]\\: message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
				{
					std::stringstream stream;
					logger< true, false > log( stream );
					log.write( system_utilities::common::details::message_level::note, "message" );
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:[A-Z ]{7}\\]\\: message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
				{
					std::stringstream stream;
					logger< true, false, false > log( stream );
					log.write( system_utilities::common::details::message_level::note, "message" );
					const std::string stream_content = stream.str();
					boost::regex message_regex( "message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
			}
			void logger_flush_tests()
			{
				std::stringstream it_is_just_a_test_on_existing_flush_method;
				logger<> l( it_is_just_a_test_on_existing_flush_method );
				BOOST_CHECK_NO_THROW( l.flush() );
			}
			void logger_note_tests()
			{
				{
					std::stringstream stream;
					logger<> l( stream );
					l.note( "note message" );
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:NOTE   \\]\\: note message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
				{
					std::stringstream stream;
					logger<> l( stream );
					l.note() << "note message";
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:NOTE   \\]\\: note message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
			}
			void logger_formatted_note_tests()
			{
				std::stringstream stream;
				logger<> l( stream );
				l.formatted_note( "new %s message %d", "formatted", 5 );
				const std::string stream_content = stream.str();
				boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:NOTE   \\]\\: new formatted message 5\n" );
				BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
			}
			void logger_warn_tests()
			{
				{
					std::stringstream stream;
					logger<> l( stream );
					l.warn( "not important message" );
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:WARNING\\]\\: not important message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
				{
					std::stringstream stream;
					logger<> l( stream );
					l.warn() << "not important message";
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:WARNING\\]\\: not important message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
			}
			void logger_formatted_warn_tests()
			{
				std::stringstream stream;
				logger<> l( stream );
				l.formatted_warn( "new %s message %d", "formatted", 5 );
				const std::string stream_content = stream.str();
				boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:WARNING\\]\\: new formatted message 5\n" );
				BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
			}
			void logger_error_tests()
			{
				{
					std::stringstream stream;
					logger<> l( stream );
					l.error( "error message" );
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:ERROR  \\]\\: error message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
				{
					std::stringstream stream;
					logger<> l( stream );
					l.error() << "error message";
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:ERROR  \\]\\: error message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
			}
			void logger_formatted_error_tests()
			{
				std::stringstream stream;
				logger<> l( stream );
				l.formatted_error( "new %s message %d", "formatted", 8 );
				const std::string stream_content = stream.str();
				boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:ERROR  \\]\\: new formatted message 8\n" );
				BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
			}
			void logger_debug_tests()
			{
				{
					std::stringstream stream;
					logger<> l( stream );
					l.debug( "debug message" );
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:DEBUG  \\]\\: debug message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
				{
					std::stringstream stream;
					logger<> l( stream );
					l.debug() << "debug message";
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:DEBUG  \\]\\: debug message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
			}
			void logger_formatted_debug_tests()
			{
				std::stringstream stream;
				logger<> l( stream );
				l.formatted_debug( "new %s message %d", "alsocr", 76 );
				const std::string stream_content = stream.str();
				boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:DEBUG  \\]\\: new alsocr message 76\n" );
				BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
			}
			void logger_fatal_tests()
			{
				{
					std::stringstream stream;
					logger<> l( stream );
					l.fatal( "fatal message" );
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:FATAL  \\]\\: fatal message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
				{
					std::stringstream stream;
					logger<> l( stream );
					l.fatal() << "fatal message";
					const std::string stream_content = stream.str();
					boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:FATAL  \\]\\: fatal message\n" );
					BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
				}
			}
			void logger_formatted_fatal_tests()
			{
				std::stringstream stream;
				logger<> l( stream );
				l.formatted_fatal( "new %s message %d", "zve", 3 );
				const std::string stream_content = stream.str();
				boost::regex message_regex( "\\[\\d{4}\\-\\w{3}\\-\\d{2} \\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{6}\\:FATAL  \\]\\: new zve message 3\n" );
				BOOST_CHECK_EQUAL( boost::regex_match( stream_content, message_regex ), true );
			}
			//
			void logger_write_performance_tests()
			{
				details::logger_write_performance_test_helper< true, true, true >( 1750, 1450 );
				details::logger_write_performance_test_helper< false, true, true >( 350, 50 );
				details::logger_write_performance_test_helper< true, true, false >( 400, 100 );
				details::logger_write_performance_test_helper< true, false, false >( 400, 100 );
				details::logger_write_performance_test_helper< true, false, true >( 1750, 1450 );
			}
		}
	}
}
