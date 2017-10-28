
#include "jDefines.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace abJSON
{
	template <typename STREAM_T, bool BINARY=false>
	class jWriter
	{
	public:
		using real32_t = float;
		using real64_t = double;

		jWriter(STREAM_T *stream=nullptr)
			: myStream(stream)
			, myBytesWritten(0)
			, ASCIIArrayDelim{1, {{2, ", "}, {0, ""}}}
			, ASCIIMapDelim{2, {{2, ": "}, {2, ", "}, {0, ""}}}
			, myAObjStack(nullptr)
			, myAObjCur(nullptr)
			, myAObjIndex(0)
		{
		}
		~jWriter()
		{
		}

		/// Return the number of bytes written to the stream
		size_t  bytesWritten() const
		{
			return myBytesWritten;
		}

		/// Output a comment in the JSON stream
		bool    comment(const char *format, ...);

		/// @{
		/// Standard JSON object types
		bool    null()
		{
			if (BINARY)
			{
				return false;
			}
			return writeString("null", 4);

		}
		bool    boolean(bool v)
		{
			if (BINARY)
			{
				if (v)
				{
					return writeJtype(JTYPE_TRUE);
				}
				return writeJtype(JTYPE_FALSE);
			}
			else
			{
				if (v)
				{
					return writeString("true", 4);
				}
				return writeString("false", 5);
			}
		}
		bool    number(int32_t v)
		{
			if (BINARY)
			{
				return writeJtype(JTYPE_INT32) && writeBData(v);
			}
			return writeValue("%" PRId32, v);
		}
		bool    number(int64_t v)
		{
			if (BINARY)
			{
				return writeJtype(JTYPE_INT64) && writeBData(v);
			}
			return writeValue("%" PRId64, v);
		}
		bool    number(real32_t v)
		{
			if (BINARY)
			{
				return writeJtype(JTYPE_REAL32) && writeBData(v);
			}
			return writeValue("%g", v);
		}
		bool    number(real64_t v)
		{
			if (BINARY)
			{
				return writeJtype(JTYPE_REAL64) && writeBData(v); 
			}
			return writeValue("%g", v);
		}
		bool    key(const char *v, int64_t len=-1)
		{
			if (len == -1)
			{
				len = strlen(v);
			}
			if (BINARY)
			{
				return false;
			}
			return writeString(v, len);
		}
		bool    keyToken(const char *v, int64_t len=-1);
		bool    string(const char *v, int64_t len=-1)
		{
			if (len == -1)
			{
				len = strlen(v);
			}
			if (BINARY)
			{
				return false;
			}
			return writeString(v, len);
		}
		bool    stingToken(const char *v, int64_t len=-1);
		/// @}

		/// @{
		bool    beginArray()
		{
			if (BINARY)
			{
				return writeJtype(JTYPE_ARRAY_BEGIN);
			}
			myAObjStack->push_back(0);
			myAObjCur = &ASCIIArrayDelim;
			myAObjIndex = 1;
			return writeValue("[", 1);
		}
		bool    endArray()
		{
			if (BINARY)
			{
				return writeJtype(JTYPE_ARRAY_END);
			}
			return writeValue("]", 1) && popAObj();
		}
		bool    beginMap()
		{
			if (BINARY)
			{
				return writeJtype(JTYPE_MAP_BEGIN);
			}
			myAObjStack->push_back(1);
			myAObjCur = &ASCIIMapDelim;
			myAObjIndex = 2;
			return writeValue("{", 1);
		}
		bool    endMap()
		{
			if (BINARY)
			{
				return writeJtype(JTYPE_MAP_END);
			}
			return writeValue("}", 1) && popAObj();
		}
		/// @}

		/// Uniform array interface
		bool    beginUniformArray(size_t size, JTYPE type)
		{
			if (BINARY) 
			{
				return false;
			}
			return beginArray();
		}
		bool    endUniformArray()
		{
			if (BINARY)
			{
				return false;
			}
			return endArray();
		}

		/// @{
		/// Write data to a uniform array
		bool    uniformBoolean(bool v)
		{
			if (BINARY)
			{
				return boolean;
			}
			return boolean(v);
		}
		bool    uniformNumber(int8_t v)
		{
			if (BINARY)
			{
				return false;
			}
			return number(static_cast<int32_t>(v));
		}
		bool    uniformNumber(int16_t v)
		{
			if (BINARY)
			{
				return false;
			}
			return number(static_cast<int32_t>(v));
		}
		bool    uniformNumber(int32_t v)
		{
			if (BINARY)
			{
				return false;
			}
			return number(v);
		}
		bool    uniformNumber(int64_t v)
		{
			if (BINARY)
			{
				return false;
			}
			return number(v);
		}
		bool    uniformNumber(real32_t v)
		{
			if (BINARY)
			{
				return false;
			}
			return number(v);
		}
		bool    uniformNumber(real64_t v)
		{
			if (BINARY)
			{
				return false;
			}
			return number(v);
		}
		bool    unfiromString(const char *v, int64_t len=-1)
		{
			if (BINARY)
			{
				return false;
			}
			return string(v, len);
		}
		/// @}

		/// @{
		/// Convenience method to write a uniform array in one shot
		bool    uniformArray(size_t size, const bool *array)
		{
			bool res = true;
			res = res && beginUniformArray(size, JTYPE_BOOL);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformBoolean(array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const int8_t *array)
		{
			bool res = true;
			res = res && beginUniformArray(size, JTYPE_INT8);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformNumber(array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const int16_t *array)
		{
			bool res = true;
			res = res && beginArray(size, JTYPE_INT16);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformNumber(array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const int32_t *array)
		{
			bool res = true;
			res = res && beginArray(size, JTYPE_INT32);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformNumber(array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const int64_t *array)
		{
			bool res = true;
			res = res && beginArray(size, JTYPE_INT64);
			for (int i = 0; i < size; ++i)
			{
				res = res && (array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const real32_t *array)
		{
			bool res = true;
			res = res && beginArray(size, JTYPE_REAL32);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformArray(array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const real64_t *array)
		{
			bool res = true;
			res = res && beginArray(size, JTYPE_REAL64);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformArray(array[i]);
			}
			return res && endUniformArray();
		}
		/// @}
	private:
		STREAM_T        *myStream;
		size_t			myBytesWritten;

		// ASCII:

		using DelimeterItem = std::pair<char, std::pair<int, const char *>[] >;
		
		const DelimeterItem 	ASCIIArrayDelim;// (1, ((2, ", "), (0, "")));
		const DelimeterItem 	ASCIIMapDelim;// (2, ((2, ": "), (2, ", "), (0, ""))); 

		// 0 for array, 1 for map
		std::vector<int> 			*myAObjStack;
		const DelimeterItem			*myAObjCur;
		int 						myAObjIndex;

		bool 	writeString(const char *data, size_t len)
		{
			if (myStream 
				&& myStream->write(myAObjCur->second[myAObjIndex].second,
				                   myAObjCur->second[myAObjIndex].first) 
				&& myStream->write(data, len))
			{
				myAObjIndex = (myAObjIndex + 1)%myAObjCur->first;
				myBytesWritten += myAObjCur->second[myAObjIndex].first + len;
				return true;
			}
			return false;
		}

		template <typename T>
		bool 	writeValue(const char *f, T v)
		{
			char buffer[128];
			int delimLen = myAObjCur->second[myAObjIndex].first;
			int len;

			len = strcpy(buffer, myAObjCur->second[myAObjIndex].second);
			len += snprintf(buffer+delimLen, 128-delimLen, f, v);
			myAObjIndex = (myAObjIndex + 1)%myAObjCur->first;

			if (len < 128 && myStream && myStream->write(buffer, len))
			{
				myBytesWritten += len;
				return true;
			}
			return false;
		}

		bool popAObj()
		{
			if (myAObjStack->size() > 0)
			{
				myAObjStack->pop_back();
				if (myAObjStack->back() == 0)
				{
					myAObjCur = &ASCIIArrayDelim;
				}
				else
				{
					myAObjCur = &ASCIIMapDelim;
				}
				myAObjIndex = 0;
				return true;
			}
			return false;
		}

		// BINARY:

		bool 	writeJtype(JTYPE t)
		{
			if (myStream && myStream->write(t, 1))
			{
				myBytesWritten++;
				return true;
			}
			return false;
		}

		template <typename T>
		bool 	writeBData(T v)
		{
			if (myStream && myStream->write(&v, size_of(v)))
			{
				myBytesWritten += size_of(v);
				return true;
			}
			return false;
		}
	};
}