#include "jDefines.h"
#include <stdint.h>
#include <ostream>

#include <iostream>

namespace abJSON
{
	class abstractJWriter : public std::ostream
	{
	protected:
		std::ostream 	*myStream;
		size_t				myBytesWritten;
	public:
		abstractJWriter(baseStream *std::ostream){}
		abstractJWriter(const jWriter &other) = delete;
		abstractJWriter &operator=(const jWriter &other) = delete;
		virtual ~abstractJWriter() = 0;

		final size_t bytesWritten() { return myBytesWritten; }

		virtual void null() = 0;
		virtual void boolean(bool v) = 0;
		virtual void number(int32_t v) = 0;
		virtual void number(int64_t v) = 0;
		virtual void number(real32_t v) = 0;
		virtual void number(real64_t v) = 0;
		virtual void key(const char *v, int64_t len=-1);
		virtual void string(const char *v, int64_t len=-1);

		virtual void beginArray();
		virtual void endArray()
		virtual void beginMap();
		virtual void endMap();
	};
}
