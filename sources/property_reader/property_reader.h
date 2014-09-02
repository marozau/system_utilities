#ifndef _SYSTEM_UTILITIES_COMMON_PROPERTY_READER_H_
#define _SYSTEM_UTILITIES_COMMON_PROPERTY_READER_H_

#include <istream>
#include <ostream>
#include <unordered_map>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

namespace system_utilities
{
	namespace tests_
	{
		namespace common
		{
			class property_reader_test_helper;
		}
	}
    namespace common
    {

		// property reader is a class that gaves you possibility to read simple configuration files
		// tests for property reader to see usage examples
		
        class property_reader
        {
			friend class system_utilities::tests_::common::property_reader_test_helper;
		public:
			typedef std::unordered_map< std::string, std::string > properties;
		private:
			std::string default_binary_path_;
			properties properties_;
            property_reader( const property_reader& );
			property_reader& operator=( const property_reader& );
		public:
			explicit property_reader();
			explicit property_reader( const std::string& file_name, const std::string& default_binary_path = "" );
			explicit property_reader( std::istream& stream, const std::string& default_binary_path = "" );
			~property_reader(){}
		public:
			void parse_istream( std::istream& stream );
		private:
			static std::string& delete_comment( std::string& str );
			bool parse_line( const std::string& str );
			bool include_line( const std::string& str );
			static bool test_on_include( const std::string& std, std::string& file_name );
			bool read_sub_property_file( const std::string& file_name );
			bool string_with_setting( const std::string& str );
			bool additional_string_with_setting( const std::string& str );
		public:
			static std::string& trim( std::string& str );
			static std::string trim(const std::string& str);
			typedef std::vector< std::string > strings;
			static strings split( const std::string& str, const std::string& delimeters = ",", const bool trim_each = true );
		public:
			size_t size() const;
            
			bool get_value( const std::string& parameter_name, const bool& default_param_value ) const;
			std::string get_value( const std::string& parameter_name, const std::string& default_param_value ) const;
			std::string get_value( const std::string& parameter_name, const char* const default_param_value = "" ) const;
			template< class result_type >
			result_type get_value( const std::string& parameter_name, const result_type& default_param_value ) const
			{
				result_type result = default_param_value;
				properties::const_iterator i = properties_.find( parameter_name );
				if ( i != properties_.end() )
					return boost::lexical_cast< result_type >( i->second );
				return result;
			}

			strings get_values( const std::string& parameter_name, const std::string& delimeters = "," ) const;

			template< class result_type >
			std::unordered_map< std::string, result_type > get_values_by_pattern( const std::string& pattern ) const
			{
				const std::string pattern_with_dot = pattern + ".";
				std::unordered_map< std::string, result_type > results;
				std::for_each( properties_.begin(), properties_.end(), [ & ]( const properties::value_type& value ) 
				{
					const std::size_t found = value.first.find( pattern_with_dot );
					if ( found != std::string::npos && found == 0 )
					{
						const result_type result = boost::lexical_cast< result_type >( value.second );
						results.insert( std::make_pair( value.first.substr( pattern_with_dot.size() ), result ) );
					}
				} );
				return results;
			}
			//
			bool set_value( const std::string& parameter_name, const std::string& value );
			template< class value_type >
			bool set_value( const std::string& parameter_name, const value_type& value )
			{
				properties_[ parameter_name ] = boost::lexical_cast< std::string >( value );
				return true;
			}
			//
			bool reset_value( const std::string& parameter_name, const std::string& value );
			template< class value_type >
			bool reset_value( const std::string& parameter_name, const value_type& value )
			{
				properties_[ parameter_name ] = boost::lexical_cast< std::string >( value );
				return true;
			}
			//
			bool delete_value( const std::string& parameter_name );
			bool rename_parameter( const std::string& old_parameter_name, const std::string& new_parameter_name );
			bool check_value( const std::string& parameter_name ) const;
			//
			void print( std::ostream& os, const std::string& format ) const;
		};
    }
}

#endif // _SYSTEM_UTILITIES_COMMON_PROPERTY_READER_H_

