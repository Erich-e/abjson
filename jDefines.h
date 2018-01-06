#ifndef __AbJSONDEFINES__
#define __AbJSONDEFINES__
namespace abJSON
{
    enum class JTYPE : char
	{
		/// No data follows the NULL token
		JNULL			= 0x00,

		/// Marks the beginning of a map object.  String/Value pairs will
		/// follow until the JMAP_END token is reached.
		JMAP_BEGIN		= 0x7b,         // '{'

		/// Marks the end of a map object
		JMAP_END		= 0x7d,         // '}'

		/// Marks the beginning of an array object.  Values will follow until
		/// the JARRAY_END token is reached.
		JARRAY_BEGIN	= 0x5b,         // '['

		/// Marks the end of an array object
		JARRAY_END		= 0x5d,         // ']'
		
		/// The following byte should be 0 or 1.  However, the use of this
		/// token to store booleans is less efficient than using
		/// JFALSE/JTRUE.@n This is primarily for completeness (and
		/// encoding of fixed boolean arrays).
		JBOOL			= 0x10,         // 0x10 | 0

		/// The following byte represents an 8 bit integer
		JINT8			= 0x11,
		/// The following 2 bytes represent an 16 bit integer
		JINT16			= 0x12,
		/// The following 4 bytes represent an 32 bit integer
		JINT32			= 0x13,
		/// The following 8 bytes represent an 64 bit integer
		JINT64			= 0x14,

		/// The following byte represents an unsigned 8 bit integer
		JUINT8  		= 0x21,
		/// The following 2 bytes represents an unsigned 16 bit integer
		JUINT16  		= 0x22,
		/// 0x23 and 0x24 are reserved for future use (32/64 bit unsigned)

		/// The following 2 bytes represent an 16 bit real (float)
		JREAL16	    = 0x18,
		/// The following 4 bytes represent an 32 bit real (float)
		JREAL32	    = 0x19,
		/// The following 8 bytes represent an 64 bit real (float)
		JREAL64	    = 0x1a,
		
		/// The data following is a binary encoded string.  The byte is
		/// followed by an encoded length followed by the bytes representing
		/// the string (the terminating NULL should not be included).
		JSTRING   		= 0x27,         // '\''
		/// A compact way of encoding [ JBOOL 0x00 ]
		JFALSE    		= 0x30,         // '0'
		/// A compact way of encoding [ JBOOL 0x01 ]
		JTRUE			= 0x31,         // '1'

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
		JTOKENDEF 		= 0x2b,         // '+'
		/// A token reference is followed by: @c [id] The id refers to a string
		/// defined by the "tokendef"
		JTOKENREF 		= 0x26,         // '*'
		/// The token undefine is followed by: @c [id] Where the @c id
		/// represents the token string to undefine.
		JTOKENUNDEF    = 0x2d,         // '-'

		/// This allows for an compact method of storing an array of uniform
		/// values.  The following byte should be one of the elementary types
		/// (bool, int8-int64, real16-real32, or string).  This is followed by
		/// the number of elements of the array (encoded as a length).  The raw
		/// data follows.
		JUNIFORM_ARRAY     = 0x40,         // '@'

		/// @private Used in parsing only
		JKEY_SEPARATOR     = 0x3a,         // ':'
		/// @private Used in parsing only
		JVALUE_SEPARATOR   = 0x2c,         // ','

		/// This should be the first byte of a binary file.  It should be
		/// followed by the 4 bytes representing the ID_MAGIC_NUMBER.  If, when
		/// reading the file, the magic number is swapped (i.e.
		/// ID_MAGIC_NUMBER_SWAP), then the file was written on a machine with
		/// different endianness and all data should be byte-swapped on
		/// loading.
		JMAGIC             = 0x7f,
	};
}
#endif