#include

namespace abJSON
{
    enum JTYPE
	{
		/// No data follows the NULL token
		JTYPE_NULL			= 0x00,

		/// Marks the beginning of a map object.  String/Value pairs will
		/// follow until the JTYPE_MAP_END token is reached.
		JTYPE_MAP_BEGIN		= 0x7b,         // '{'

		/// Marks the end of a map object
		JTYPE_MAP_END		= 0x7d,         // '}'

		/// Marks the beginning of an array object.  Values will follow until
		/// the JTYPE_ARRAY_END token is reached.
		JTYPE_ARRAY_BEGIN	= 0x5b,         // '['

		/// Marks the end of an array object
		JTYPE_ARRAY_END		= 0x5d,         // ']'
		
		/// The following byte should be 0 or 1.  However, the use of this
		/// token to store booleans is less efficient than using
		/// JTYPE_FALSE/JTYPE_TRUE.@n This is primarily for completeness (and
		/// encoding of fixed boolean arrays).
		JTYPE_BOOL			= 0x10,         // 0x10 | 0

		/// The following byte represents an 8 bit integer
		JTYPE_INT8			= 0x11,
		/// The following 2 bytes represent an 16 bit integer
		JTYPE_INT16			= 0x12,
		/// The following 4 bytes represent an 32 bit integer
		JTYPE_INT32			= 0x13,
		/// The following 8 bytes represent an 64 bit integer
		JTYPE_INT64			= 0x14,

		/// The following byte represents an unsigned 8 bit integer
		JTYPE_UINT8  		= 0x21,
		/// The following 2 bytes represents an unsigned 16 bit integer
		JTYPE_UINT16  		= 0x22,
		/// 0x23 and 0x24 are reserved for future use (32/64 bit unsigned)

		/// The following 2 bytes represent an 16 bit real (float)
		JTYPE_REAL16	    = 0x18,
		/// The following 4 bytes represent an 32 bit real (float)
		JTYPE_REAL32	    = 0x19,
		/// The following 8 bytes represent an 64 bit real (float)
		JTYPE_REAL64	    = 0x1a,
		
		/// The data following is a binary encoded string.  The byte is
		/// followed by an encoded length followed by the bytes representing
		/// the string (the terminating NULL should not be included).
		JTYPE_STRING   		= 0x27,         // '\''
		/// A compact way of encoding [ JTYPE_BOOL 0x00 ]
		JTYPE_FALSE    		= 0x30,         // '0'
		/// A compact way of encoding [ JTYPE_BOOL 0x01 ]
		JTYPE_TRUE			= 0x31,         // '1'

		/// Though gzip compression on a stream can reduce redundancy, storage
		/// of command strings can be simplified by using "token-strings".  On
		/// the first use of a binary string, a token-define can be used.  This
		/// assigns an integer value to the string.  The string can then be
		/// referenced by the token-reference identifier.
		///
		/// A token definition is followed by: <tt>[id] [length]
		/// [string-data]</tt>
		///
		/// If a string already exists with this id, its definition will be
		/// replaced with the new definition.
		JTYPE_TOKENDEF 		= 0x2b,         // '+'
		/// A token reference is followed by: @c [id] The id refers to a string
		/// defined by the "tokendef"
		JTYPE_TOKENREF 		= 0x26,         // '*'
		/// The token undefine is followed by: @c [id] Where the @c id
		/// represents the token string to undefine.
		JTYPE_TOKENUNDEF    = 0x2d,         // '-'

		/// This allows for an compact method of storing an array of uniform
		/// values.  The following byte should be one of the elementary types
		/// (bool, int8-int64, real16-real32, or string).  This is followed by
		/// the number of elements of the array (encoded as a length).  The raw
		/// data follows.
		JTYPE_UNIFORM_ARRAY     = 0x40,         // '@'

		/// @private Used in parsing only
		JTYPE_KEY_SEPARATOR     = 0x3a,         // ':'
		/// @private Used in parsing only
		JTYPE_VALUE_SEPARATOR   = 0x2c,         // ','

		/// This should be the first byte of a binary file.  It should be
		/// followed by the 4 bytes representing the ID_MAGIC_NUMBER.  If, when
		/// reading the file, the magic number is swapped (i.e.
		/// ID_MAGIC_NUMBER_SWAP), then the file was written on a machine with
		/// different endianness and all data should be byte-swapped on
		/// loading.
		JTYPE_MAGIC             = 0x7f,
	};
}
