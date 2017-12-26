#include "jDefines.h"
#include <inttypes.h>
#include <limits>
#include <map>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>


#include <iostream>

namespace abJSON
{
	class jData
	{
	public:
		static std::pair<int, const char *>  *getArrayDelimiter();

		static std::pair<int, const char *>  *getMapDelimiter();
	};

	template <typename STREAM_T, bool BINARY=false>
	class jWriter
	{
	public:
		using real32_t = float;
		using real64_t = double;
		enum errorState {
			NO_ERROR = 0,
			INVALID_INPUT
		};

		jWriter(STREAM_T *stream=nullptr)
			: myStream(stream)
			, myBytesWritten(0)
			, myErrorState(NO_ERROR)
			, ASCIIArrayDelim(1, jData::getArrayDelimiter())
			, ASCIIMapDelim(2, jData::getMapDelimiter())
			, myAObjCur(nullptr)
			, myAObjIndex(0)
			, strCount(0)
		{
			if (BINARY)
			{
				writeJType(JTYPE::JMAGIC);
			}
		}
		~jWriter()
		{
			delete[] ASCIIArrayDelim.second;
			delete[] ASCIIMapDelim.second;
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
				return writeJType(JTYPE::JNULL);
			}
			return writeValue("null", nullptr);
		}
		bool    boolean(bool v)
		{
			if (BINARY)
			{
				if (v)
				{
					return writeJType(JTYPE::JTRUE);
				}
				return writeJType(JTYPE::JFALSE);
			}
			else
			{
				if (v)
				{
					return writeValue("true", nullptr);
				}
				return writeValue("false", nullptr);
			}
		}
		bool    number(int32_t v)
		{
			if (BINARY)
			{
				if (isSmallInt<int16_t>(v))
				{
					if (isSmallInt<int8_t>(v))
					{
						return writeJType(JTYPE::JINT8)
						       && writeBData(static_cast<int8_t>(v)); 
					}
					return writeJType(JTYPE::JINT16)
						   && writeBData(static_cast<int16_t>(v));
				}
				return writeJType(JTYPE::JINT32) && writeBData(v);
			}
			return writeValue("%" PRId32, v);
		}
		bool    number(int64_t v)
		{
			if (BINARY)
			{
				if (isSmallInt<int32_t>(v))
				{
					return number(static_cast<int32_t>(v));	
				}
				return writeJType(JTYPE::JINT64) && writeBData(v);
			}
			return writeValue("%" PRId64, v);
		}
		bool    number(real32_t v)
		{
			if (BINARY)
			{
				return writeJType(JTYPE::JREAL32) && writeBData(v);
			}
			return writeValue("%g", v);
		}
		bool    number(real64_t v)
		{
			if (BINARY)
			{
				return writeJType(JTYPE::JREAL64) && writeBData(v); 
			}
			return writeValue("%g", v);
		}
		bool    key(const char *v, int64_t len=-1)
		{
			return string(v, len);
		}
		bool    keyToken(const char *v, int64_t len=-1)
		{
			return stringToken(v, len);
		}
		bool    string(const char *v, int64_t len=-1)
		{
			if (len == -1)
			{
				len = strlen(v);
			}
			if (BINARY)
			{
				return writeJType(JTYPE::JSTRING)
					   && number(len)
					   && writeBString(v, len);
			}
			return writeString(v, len);
		}
		bool    stringToken(const char *v, int64_t len=-1)
		{
			if (len == -1)
			{
				len = strlen(v);
			}
			if (BINARY)
			{
				auto search = strTokens.find(v);
				if(search == strTokens.end())
				{
					strTokens[v] = strCount;
					strCount++;
					return writeJType(JTYPE::JTOKENDEF)
						   && number(strCount-1)
						   && string(v, len);
				}
				else
				{
					return writeJType(JTYPE::JTOKENREF)
						   && number(search->second);
				}
			}
			return string(v, len);
		}
		/// @}

		/// @{
		bool    beginArray()
		{
			if (BINARY)
			{
				return writeJType(JTYPE::JARRAY_BEGIN);
			}
			bool res = writeValue("[", nullptr);
			myAObjStack.push_back(0);
			myAObjCur = &ASCIIArrayDelim;
			myAObjIndex = 1;
			return res;
		}
		bool    endArray()
		{
			if (BINARY)
			{
				return writeJType(JTYPE::JARRAY_END);
			}
			return writeValue("]", nullptr, false) && popAObj();
		}
		bool    beginMap()
		{
			if (BINARY)
			{
				return writeJType(JTYPE::JMAP_BEGIN);
			}
			bool res = writeValue("{", nullptr);
			myAObjStack.push_back(1);
			myAObjCur = &ASCIIMapDelim;
			myAObjIndex = 3;
			return res;
		}
		bool    endMap()
		{
			if (BINARY)
			{
				return writeJType(JTYPE::JMAP_END);
			}
			return writeValue("}", nullptr, false) && popAObj();
		}
		/// @}

		/// Uniform array interface
		bool    beginUniformArray(size_t size, JTYPE type)
		{
			if (BINARY) 
			{
				return writeJType(JTYPE::JUNIFORM_ARRAY)
					   && writeJType(type)
					   && number(size);
			}
			return beginArray();
		}
		bool    endUniformArray()
		{
			if (BINARY)
			{
				return true;
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
				return writeBData(v);
			}
			return number(static_cast<int32_t>(v));
		}
		bool    uniformNumber(int16_t v)
		{
			if (BINARY)
			{
				return writeBData(v);
			}
			return number(static_cast<int32_t>(v));
		}
		bool    uniformNumber(int32_t v)
		{
			if (BINARY)
			{
				// if (v < )
				return writeBData(v);
			}
			return number(v);
		}
		bool    uniformNumber(int64_t v)
		{
			if (BINARY)
			{
				return writeBData(v);
			}
			return number(v);
		}
		bool    uniformNumber(real32_t v)
		{
			if (BINARY)
			{
				return writeBData(v);
			}
			return number(v);
		}
		bool    uniformNumber(real64_t v)
		{
			if (BINARY)
			{
				return writeBData(v);
			}
			return number(v);
		}
		bool    unfiromString(const char *v, int64_t len=-1)
		{
			if (BINARY)
			{
				if (len == -1)
				{
					len = strlen(v);
				}
				return number(len) && writeBData(v);
			}
			return string(v, len);
		}
		/// @}

		/// @{
		/// Convenience method to write a uniform array in one shot
		bool    uniformArray(size_t size, const bool *array)
		{
			bool res = true;
			res = res && beginUniformArray(size, JTYPE::JBOOL);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformBoolean(array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const int8_t *array)
		{
			bool res = true;
			res = res && beginUniformArray(size, JTYPE::JINT8);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformNumber(array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const int16_t *array)
		{
			bool res = true;
			res = res && beginArray(size, JTYPE::JINT16);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformNumber(array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const int32_t *array)
		{
			bool res = true;
			res = res && beginArray(size, JTYPE::JINT32);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformNumber(array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const int64_t *array)
		{
			bool res = true;
			res = res && beginArray(size, JTYPE::JINT64);
			for (int i = 0; i < size; ++i)
			{
				res = res && (array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const real32_t *array)
		{
			bool res = true;
			res = res && beginArray(size, JTYPE::JREAL32);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformArray(array[i]);
			}
			return res && endUniformArray();
		}
		bool    uniformArray(size_t size, const real64_t *array)
		{
			bool res = true;
			res = res && beginArray(size, JTYPE::JREAL64);
			for (int i = 0; i < size; ++i)
			{
				res = res && uniformArray(array[i]);
			}
			return res && endUniformArray();
		}
		/// @}

		/// State Functions
		errorState getErrorState() {
			return myErrorState;
		}

		explicit operator bool(){
			return myErrorState != NO_ERROR;
		}

	private:
		STREAM_T        *myStream;
		size_t			myBytesWritten;
		int myErrorState;

		// ASCII:

		using DelimeterItem = std::pair<char, std::pair<int, const char *> *>;
		
		const DelimeterItem 	ASCIIArrayDelim;// (1, ((2, ", "), (0, "")));
		const DelimeterItem 	ASCIIMapDelim;// (2, ((2, ": "), (2, ", "), (0, "")));

		// 0 for array, 1 for map
		std::vector<int> 			myAObjStack;
		const DelimeterItem			*myAObjCur;
		int 						myAObjIndex;

		// BINARY
		int strCount = 0;
		std::map<std::string, int> strTokens;

		bool 	writeString(const char *data, size_t len, bool delim=true)
		{
			char buffer[128];
			int blen = 0;
			bool res = true;
			if (myStream)
			{
				if (delim && myAObjCur)
				{
					if(!myStream->write(myAObjCur->second[myAObjIndex].second,
										  myAObjCur->second[myAObjIndex].first))
						res = false;
					myAObjIndex = (myAObjIndex + 1)%myAObjCur->first;
					myBytesWritten += myAObjCur->second[myAObjIndex].first + len;
				}
				myStream->write("\"", 1);
				myBytesWritten++;
				for (int i = 0; i < len; i++) {
					// write the string 
					if (blen > 127) {
						if (!myStream->write(buffer, blen))
							res = false;
						myBytesWritten += blen;
						blen = 0;
					}
					switch(data[i]) {
						case '\"':
							buffer[blen++] = '\\';
							buffer[blen++] = '\"';
							break;
						case '\\':
							buffer[blen++] = '\\';
							buffer[blen++] = '\\';
							break;
						case '/':
							buffer[blen++] = '\\';
							buffer[blen++] = '/';
							break;
						case '\b':
							buffer[blen++] = '\\';
							buffer[blen++] = 'b';
							break;
						case '\f':
							buffer[blen++] = '\\';
							buffer[blen++] = 'f';
							break;
						case '\n':
							buffer[blen++] = '\\';
							buffer[blen++] = 'n';
							break;
						case '\r':
							buffer[blen++] = '\\';
							buffer[blen++] = 'r';
							break;
						case '\t':
							buffer[blen++] = '\\';
							buffer[blen++] = 't';
							break;
						default:
							buffer[blen++] = data[i];
					}
				}
				if (blen > 0) {
					if(!myStream->write(buffer, blen))
						res = false;
					myBytesWritten += blen;
				}
				myStream->write("\"", 1);
				myBytesWritten++;
			}
			return res;
		}

		template <typename T>
		bool 	writeValue(const char *f, T v, bool delim = true)
		{
			char buffer[128];
			int len = 0;
			if (delim && myAObjCur)
			{
				len = myAObjCur->second[myAObjIndex].first;
				strncpy(buffer, myAObjCur->second[myAObjIndex].second, len);
				myAObjIndex = (myAObjIndex + 1)%myAObjCur->first;
			}

			len += snprintf(buffer+len, 127-len, f, v);
			if (len < 128 && myStream && myStream->write(buffer, len))
			{
				myBytesWritten += len;
				return true;
			}
			return false;
		}

		bool popAObj()
		{
			if (myAObjStack.size() > 0)
			{
				myAObjStack.pop_back();
				if (myAObjStack.back() == 0)
				{
					myAObjCur = &ASCIIArrayDelim;
					myAObjIndex = 0;
				}
				else
				{
					myAObjCur = &ASCIIMapDelim;
					myAObjIndex = 1;
				}
				return true;
			}
			return false;
		}

		// BINARY:

		bool 	writeJType(JTYPE t)
		{
			static_assert(sizeof(JTYPE) == sizeof(char), "wrong size for JTYPE");
			if (myStream && myStream->write((const char *)&t, sizeof(JTYPE)))
			{
				myBytesWritten++;
				return true;
			}
			return false;
		}

		template <typename T>
		bool 	writeBData(T v)
		{
			if (myStream && myStream->write((const char *)&v, sizeof(v)))
			{
				myBytesWritten += sizeof(v);
				return true;
			}
			return false;
		}

		bool 	writeBString(const char* data, size_t len)
		{
			if (myStream && myStream->write(data, len))
			{
				myBytesWritten += len;
				return true;
			}
			return false;
		}

		template <typename T>
		bool isSmallInt(T v) {
			return std::numeric_limits<T>::min() < v && v < std::numeric_limits<T>::max();
		}
	};
}