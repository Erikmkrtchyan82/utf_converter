#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iterator>

using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;

using UTF_8_type = unsigned char;
using UTF_16_type = char16_t;
using UTF_32_type = char32_t;

//	To find out UTF formats
enum class UTF {
	//	If encoding format is invalid
	UTF_ERROR = 0,

	UTF_8, UTF_16, UTF_32
};

UTF find_utf( const string& );

//	Throws exception
//	If everything is okay does nothing
void check_invalid_inputs( char* [], ifstream&, ofstream& );

//	Throws exception, if format of text in input file is not UTF-8
void decode_utf_8( vector<unsigned int>&, ifstream& );

//	Throws exception, if format of text in input file is not UTF-16
void decode_utf_16( vector<unsigned int>&, ifstream& );

//	Throws exception, if format of text in input file is not UTF-32
void decode_utf_32( vector<unsigned int>&, ifstream& );

void read_from_file( UTF&, vector<unsigned int>&, ifstream& );

template<typename InputIterator>
void encode_utf_8( vector<unsigned int>::iterator& begin, vector<unsigned int>::iterator& end, InputIterator& out ) {

	for ( ; begin != end; ++begin, ++out ) {
		//	Needs one byte to encode
		if ( *begin <= 0x7F ) {
			*out = *begin;
		}
		//	Needs two bytes to encode
		else if ( *begin <= 0x7FF ) {
			auto first = ( ( ( *begin >> 6 ) & 0x1F ) | 0xC0 );
			auto second = ( *begin & 0x3F ) | 0x80;

			*out = first;
			*( ++out ) = second;
		}
		//	Needs three bytes to encode
		else if ( *begin <= 0x7FFF ) {
			auto first = ( ( ( *begin >> 8 >> 4 ) & 0xF ) | 0xE0 );
			auto second = ( ( ( *begin >> 6 ) & 0x3F ) | 0x80 );
			auto third = ( *begin & 0x3F ) | 0x80;

			*out = first;
			*( ++out ) = second;
			*( ++out ) = third;
		}
		//	Needs four bytes to encode
		else if ( *begin <= 0xFFFF ) {
			auto first = ( ( ( *begin >> 16 >> 2 ) & 0x7 ) | 0x1E );
			auto second = ( ( ( *begin >> 8 >> 4 ) & 0x3F ) | 0x80 );
			auto third = ( ( ( *begin >> 6 ) & 0x3F ) | 0x80 );
			auto forth = ( *begin & 0x3F ) | 0x80;

			*out = first;
			*( ++out ) = second;
			*( ++out ) = third;
			*( ++out ) = forth;
		}
	}
}

//	Throws exception
template<typename InputIterator>
void encode_utf_16( vector<unsigned int>::iterator& begin, vector<unsigned int>::iterator& end, InputIterator& out ) {

	for ( ; begin != end; ++begin, ++out ) {
		if ( *begin >= 0xD800 && *begin <= 0xDFFF ) {
			string error_message = "Can't convert ";
			error_message += *begin + " to UTF-16\n";

			throw std::invalid_argument( error_message );
		}
		//	Needs four bytes to encode
		if ( *begin >= 0x010000 && *begin <= 0x10FFFF )
		{
			auto copy = *begin - 0x10000;
			auto high = ( copy >> 10 ) + 0xD800;
			auto low = ( copy & 0x3FF ) + 0xDC00;

			*out = high;
			*( ++out ) = low;
		}
		//	Needs two bytes to encode
		else {
			*out = *begin;
		}
	}
}

template<typename InputIterator>
void encode_utf_32( vector<unsigned int>::iterator& begin, vector<unsigned int>::iterator& end, InputIterator& out ) {
	for ( ; begin != end; ++begin, ++out ) {
		*out = *begin;
	}
}

template<typename InputIterator>
void write_in_file( InputIterator begin, InputIterator end, ofstream& output_file ) {
	for ( ; begin != end && *begin != 0; ++begin ) {
		output_file.write( (char*)( &*begin ), sizeof( typename std::iterator_traits<InputIterator>::value_type ) );
	}
}

//	Throws exception
template <typename InputIterator, typename OutputIterator>
void utf_convert( InputIterator begin, InputIterator end, OutputIterator out ) {

	string error_message = "Something went wrong in function \"utf_convert\"\n";

	if ( typeid( typename std::iterator_traits<OutputIterator>::value_type ) == typeid( UTF_8_type ) ) {
		encode_utf_8( begin, end, out );
		return;
	}
	if ( typeid( typename std::iterator_traits<OutputIterator>::value_type ) == typeid( UTF_16_type ) ) {
		encode_utf_16( begin, end, out );
		return;
	}
	if ( typeid( typename std::iterator_traits<OutputIterator>::value_type ) == typeid( UTF_32_type ) ) {
		encode_utf_32( begin, end, out );
		return;
	}
	throw std::invalid_argument( error_message );
}
