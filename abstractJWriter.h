#ifndef ABJSON_ABSTRACTJWRITER
#define ABJSON_ABSTRACTJWRITER
#include "jDefines.h"
#include "jState.h"
#include <stdint.h>
#include <ostream>

#include <iostream>

namespace abJSON
{
	class abstractJWriter
	{
	protected:
		std::ostream 		*myStream;
		size_t				myBytesWritten;
		jStateManager		myState;
	public:
		abstractJWriter(std::ostream *baseStream);
		abstractJWriter(const abstractJWriter &other) = delete;
		abstractJWriter &operator=(const abstractJWriter &other) = delete;
		virtual ~abstractJWriter() = 0;

		size_t bytesWritten();

		virtual void null() = 0;
		virtual void boolean(bool v) = 0;
		virtual void number(int32_t v) = 0;
		virtual void number(int64_t v) = 0;
		virtual void number(float v) = 0;
		virtual void number(double v) = 0;
		virtual void key(const char *v, int64_t len=-1) = 0;
		virtual void string(const char *v, int64_t len=-1) = 0;

		virtual void beginArray() = 0;
		virtual void endArray() = 0;
		virtual void beginMap() = 0;
		virtual void endMap() = 0;
	};
}
#endif