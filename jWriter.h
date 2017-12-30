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
			STREAM_ERROR,
			BAD_INPUT
		};

		jWriter(STREAM_T *stream=nullptr, bool prettyPrint=false)
			: myStream(stream)
			, myBytesWritten(0)
			, myErrorState(NO_ERROR)
			, ASCIIArrayDelim(1, jData::getArrayDelimiter())
			, ASCIIMapDelim(2, jData::getMapDelimiter())
			, myAObjCur(nullptr)
			, myAObjIndex(0)
			, prettyPrint(prettyPrint)
			, strCount(0)
		{
			if (BINARY)
			{
				writeJType(JTYPE::JMAGIC);
				writeBData(0x624a534e);
			}
		}
		jWriter(const jWriter &other) = delete;
		jWriter &operator=(const jWriter &other) = delete;
		~jWriter()
		{
			if (prettyPrint && !(BINARY))
			{
				//End files with a newline
				newLine();
			}
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
		void    null()
		{
			if (BINARY)
			{
				writeJType(JTYPE::JNULL);
			}
			else
			{
				writeValue("null", nullptr);
			}
		}
		void    boolean(bool v)
		{
			if (BINARY)
			{
				if (v)
				{
					writeJType(JTYPE::JTRUE);
				}
				else
				{
					writeJType(JTYPE::JFALSE);
				}
			}
			else
			{
				if (v)
				{
					writeValue("true", nullptr);
				}
				else
				{
				writeValue("false", nullptr);
				}
			}
		}
		void    number(int32_t v)
		{
			if (BINARY)
			{
				if (isSmallInt<int16_t>(v))
				{
					if (isSmallInt<int8_t>(v))
					{
						writeJType(JTYPE::JINT8);
						writeBData(static_cast<int8_t>(v)); 
					}
					else
					{
						writeJType(JTYPE::JINT16);
						writeBData(static_cast<int16_t>(v));
					}
				}
				else
				{
					writeJType(JTYPE::JINT32);
					writeBData(v);
				}
			}
			else
			{
				writeValue("%" PRId32, v);
			}
		}
		void    number(int64_t v)
		{
			if (BINARY)
			{
				if (isSmallInt<int32_t>(v))
				{
					number(static_cast<int32_t>(v));	
				}
				else
				{
					writeJType(JTYPE::JINT64);
					writeBData(v);
				}
			}
			else
			{
				writeValue("%" PRId64, v);
			}
		}
		void    number(real32_t v)
		{
			if (BINARY)
			{
				writeJType(JTYPE::JREAL32);
				writeBData(v);
			}
			else
			{
				writeValue("%g", v);
			}
		}
		void    number(real64_t v)
		{
			if (BINARY)
			{
				writeJType(JTYPE::JREAL64);
				writeBData(v); 
			}
			else
			{
				writeValue("%g", v);
			}
		}
		void    key(const char *v, int64_t len=-1)
		{
			string(v, len);
		}
		void    keyToken(const char *v, int64_t len=-1)
		{
			stringToken(v, len);
		}
		void    string(const char *v, int64_t len=-1)
		{
			if (len == -1)
			{
				len = strlen(v);
			}
			if (BINARY)
			{
				writeJType(JTYPE::JSTRING);
				writeLength(len);
				writeBString(v, len);
			}
			else
			{
				writeString(v, len);
			}
		}
		void    stringToken(const char *v, int64_t len=-1)
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
					writeJType(JTYPE::JTOKENDEF);
					writeLength(strCount-1);
					writeLength(len);
					writeBString(v, len);
				}
				else
				{
					writeJType(JTYPE::JTOKENREF);
					number(search->second);
				}
			}
			else
			{
				string(v, len);
			}
		}
		/// @}

		/// @{
		void    beginArray()
		{
			if (BINARY)
			{
				writeJType(JTYPE::JARRAY_BEGIN);
			}
			else
			{
				writeValue("[", nullptr);
				myAObjStack.push_back(0);
				myAObjCur = &ASCIIArrayDelim;
				myAObjIndex = 1;
				indentLevel++;
			}
		}
		void    endArray()
		{
			if (BINARY)
			{
				writeJType(JTYPE::JARRAY_END);
			}
			else
			{
				if (myAObjCur != &ASCIIArrayDelim)
				{
					myErrorState = BAD_INPUT;
				}
				indentLevel--;
				newLine();
				writeValue("]", nullptr, false);
				popAObj();
			}
		}
		void    beginMap()
		{
			if (BINARY)
			{
				return writeJType(JTYPE::JMAP_BEGIN);
			}
			else
			{
				writeValue("{", nullptr);
				myAObjStack.push_back(1);
				myAObjCur = &ASCIIMapDelim;
				myAObjIndex = 2;
				indentLevel++;
			}
		}
		void    endMap()
		{
			if (BINARY)
			{
				writeJType(JTYPE::JMAP_END);
			}
			else
			{
				if (myAObjCur != &ASCIIMapDelim || myAObjIndex == 1)
				{		
					myErrorState = BAD_INPUT;
				}
				indentLevel--;
				newLine();
				writeValue("}", nullptr, false);
				popAObj();
			}
		}
		/// @}

		/// Uniform array interface
		void    beginUniformArray(size_t size, JTYPE type)
		{
			if (BINARY) 
			{
				writeJType(JTYPE::JUNIFORM_ARRAY);
				writeJType(type);
				writeLength(size);
			}
			return beginArray();
		}
		void    endUniformArray()
		{
			if (BINARY)
			{
				;
			}
			else
			{
				endArray();
			}
		}

		/// @{
		/// Write data to a uniform array
		void    uniformBoolean(bool v)
		{
			if (BINARY)
			{
				writeBData(v);
			}
			else
			{
				boolean(v);
			}
		}
		void    uniformNumber(int8_t v)
		{
			if (BINARY)
			{
				writeBData(v);
			}
			else
			{
				number(static_cast<int32_t>(v));
			}
		}
		void    uniformNumber(int16_t v)
		{
			if (BINARY)
			{
				writeBData(v);
			}
			else
			{
				number(static_cast<int32_t>(v));
			}
		}
		void    uniformNumber(int32_t v)
		{
			if (BINARY)
			{
				writeBData(v);
			}
			else
			{
				number(v);
			}
		}
		void    uniformNumber(int64_t v)
		{
			if (BINARY)
			{
				writeBData(v);
			}
			else
			{
				number(v);
			}
		}
		void    uniformNumber(real32_t v)
		{
			if (BINARY)
			{
				writeBData(v);
			}
			else
			{
				number(v);
			}
		}
		void    uniformNumber(real64_t v)
		{
			if (BINARY)
			{
				writeBData(v);
			}
			else
			{
				number(v);
			}
		}
		void    unfiromString(const char *v, int64_t len=-1)
		{
			if (len == -1)
			{
				len = strlen(v);
			}
			if (BINARY)
			{
				number(len);
				writeBString(v, len);
			}
			else
			{
				string(v, len);
			}
		}
		/// @}

		/// @{
		/// Convenience method to write a uniform array in one shot
		void    uniformArray(size_t size, const bool *array)
		{
			beginUniformArray(size, JTYPE::JBOOL);
			for (int i = 0; i < size; ++i)
			{
				uniformBoolean(array[i]);
			}
			endUniformArray();
		}
		void    uniformArray(size_t size, const int8_t *array)
		{
			beginUniformArray(size, JTYPE::JINT8);
			for (int i = 0; i < size; ++i)
			{
				uniformNumber(array[i]);
			}
			endUniformArray();
		}
		void    uniformArray(size_t size, const int16_t *array)
		{
			beginArray(size, JTYPE::JINT16);
			for (int i = 0; i < size; ++i)
			{
				uniformNumber(array[i]);
			}
			endUniformArray();
		}
		void    uniformArray(size_t size, const int32_t *array)
		{
			beginArray(size, JTYPE::JINT32);
			for (int i = 0; i < size; ++i)
			{
				uniformNumber(array[i]);
			}
			endUniformArray();
		}
		void    uniformArray(size_t size, const int64_t *array)
		{
			beginArray(size, JTYPE::JINT64);
			for (int i = 0; i < size; ++i)
			{
				uniformArray(array[i]);
			}
			endUniformArray();
		}
		void    uniformArray(size_t size, const real32_t *array)
		{
			beginArray(size, JTYPE::JREAL32);
			for (int i = 0; i < size; ++i)
			{
				uniformArray(array[i]);
			}
			endUniformArray();
		}
		void    uniformArray(size_t size, const real64_t *array)
		{
			beginArray(size, JTYPE::JREAL64);
			for (int i = 0; i < size; ++i)
			{
				uniformArray(array[i]);
			}
			endUniformArray();
		}
		/// @}

		/// State Functions 
		errorState getErrorState() {
			return myErrorState;
		}

		void clear() {
			myErrorState = NO_ERROR;
		}

		explicit operator bool(){
			return myErrorState != NO_ERROR;
		}

	private:
		STREAM_T        		*myStream;
		size_t					myBytesWritten;
		int						myErrorState;

		// ASCII:

		using DelimeterItem = std::pair<char, std::pair<int, const char *> *>;
		
		const DelimeterItem 	ASCIIArrayDelim;// (1, ((2, ", "), (0, "")));
		const DelimeterItem 	ASCIIMapDelim;// (2, ((2, ": "), (2, ", "), (0, "")));

		// 0 for array, 1 for map
		std::vector<int> 		myAObjStack;
		const DelimeterItem		*myAObjCur;
		int 					myAObjIndex;

		bool 					prettyPrint;
		int 					indentLevel;

		// BINARY
		int strCount = 0;
		std::map<std::string, int> strTokens;

		void 	writeString(const char *data, size_t len, bool delim=true)
		{
			char buffer[128];
			int blen = 0;
			if (myStream)
			{
				if (delim && myAObjCur)
				{
					sprintf(buffer, "%s", myAObjCur->second[myAObjIndex].second);
					blen += myAObjCur->second[myAObjIndex].first;
					myAObjIndex = (myAObjIndex + 1)%myAObjCur->first;
					if (prettyPrint && (myAObjCur == &ASCIIArrayDelim || myAObjIndex == 1))
					{
						addNewLineToBuffer(buffer + blen);
						blen += 2*indentLevel + 1;
					}
				}
				sprintf(buffer+blen, "\"");
				blen++;
				for (int i = 0; i < len; i++) {
					// write the string 
					if (blen > 127) {
						if (!myStream->write(buffer, blen))
						{
							myErrorState = STREAM_ERROR;
							return;
						}
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
					{
						myErrorState = STREAM_ERROR;
						return;
					}
					myBytesWritten += blen;
				}
				if (!myStream->write("\"", 1))
				{
					myErrorState = STREAM_ERROR;
					return;
				}
				myBytesWritten++;
			}
			else
			{
				myErrorState = STREAM_ERROR;
			}
		}

		template <typename T>
		void 	writeValue(const char *f, T v, bool delim = true)
		{
			char buffer[128];
			int len = 0;
			if (delim && myAObjCur)
			{
				len = myAObjCur->second[myAObjIndex].first;
				strncpy(buffer, myAObjCur->second[myAObjIndex].second, len);
				myAObjIndex = (myAObjIndex + 1)%myAObjCur->first;
				if (prettyPrint && (myAObjCur == &ASCIIArrayDelim || myAObjIndex == 1))
				{
					addNewLineToBuffer(buffer + len);
					len += 2*indentLevel + 1;
				}
			}
			len += snprintf(buffer+len, 127-len, f, v);
			if (len < 128)
			{
				if (myStream && myStream->write(buffer, len))
				{
					myBytesWritten += len;
				}
				else
				{
					myErrorState = STREAM_ERROR;
				}
			}
			else
			{
				myErrorState = BAD_INPUT;
			}
		}

		void popAObj()
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
			}
		}

		void addNewLineToBuffer(char *buffer)
		{
			buffer[0] = '\n';
			for (int i = 0; i < indentLevel; i++)
			{
				buffer[2*i+1] = ' ';
				buffer[2*i+2] = ' ';
			}
		}

		void newLine()
		{
			char buffer[128];
			int len = 2*indentLevel + 1;
			addNewLineToBuffer(buffer);
			if (myStream && myStream->write(buffer, len))
			{
				myBytesWritten += len;
			}
			else
			{
				myErrorState = STREAM_ERROR;
			}
		}

		// BINARY:

		void 	writeJType(JTYPE t)
		{
			static_assert(sizeof(JTYPE) == sizeof(char), "wrong size for JTYPE");
			if (myStream && myStream->write((const char *)&t, sizeof(JTYPE)))
			{
				myBytesWritten++;
			}
			else
			{
				myErrorState = STREAM_ERROR;
			}
		}

		template <typename T>
		void 	writeBData(T v)
		{
			if (myStream && myStream->write((const char *)&v, sizeof(v)))
			{
				myBytesWritten += sizeof(v);
			}
			else
			{
				myErrorState = STREAM_ERROR;
			}
		}

		void 	writeBString(const char* data, size_t len)
		{
			if (myStream && myStream->write(data, len))
			{
				myBytesWritten += len;
			}
			else
			{
				myErrorState = STREAM_ERROR;
			}
		}

		void writeLength(int64_t len)
		{
			if (len < 0xf1)
			{
				writeBData(static_cast<int8_t>(len));
			}
			else if (len < 0xffff)
			{
				writeBData(0xf2);
				writeBData(static_cast<int16_t>(len));
			}
			else if (len > 0xffffffff)
			{
				writeBData(0xf4);
				writeBData(static_cast<int32_t>(len));
			}
			else
			{
				writeBData(0xf8);
				writeBData(len);
			}
		}

		template <typename T, typename VALUE_T>
		bool isSmallInt(VALUE_T v) {
			return std::numeric_limits<T>::min() < v && v < std::numeric_limits<T>::max();
		}
	};
}