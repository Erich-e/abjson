#include "ASCIIJWriter.h"
#include <inttypes.h>

using namespace abJSON;

ASCIIJWriter::ASCIIJWriter(baseStream *std::ostream, bool prettyPrint = true)
  : myStream(baseStream)
  , prettyPrint(prettyPrint)
{

}

~ASCIIJWriter()
{
  delete myStream;
}

void ASCIIJWriter::null()
{
  writeData("null", nullptr);
}

void ASCIIJWriter::boolean(bool v)
{
  if (v)
  {
    writeData("true", nullptr);
  }
  else
  {
    writeData("false", nullptr);
  }
}

void ASCIIJWriter::number(int32_t)
{
  writeData<int_32>("%" PRId32, v);
}

void ASCIIJWriter::number(int64_t)
{
  writeData<int_64>("%" PRId64, v);
}

void ASCIIJWriter::number(real32_t)
{
  writeData<real32_t>("%g", v);
}

void ASCIIJWriter::number(real64_t)
{
  writeData<real_64>("%g", v);
}

void ASCIIJWriter::key(const char *v, int64_t len=-1)
{
  writeString(data, len);
}

void ASCIIJWriter::string(const char *v, int64_t len=-1)
{
  writeString(data, len);
}

void ASCIIJWriter::beginArray()
{
  writeData("[", nullptr);
}

void ASCIIJWriter::endArray()
{
  writeData("]", nullptr);
}

void ASCIIJWriter::beginMap()
{
  writeData("{", nullptr);
}

void ASCIIJWriter::endMap()
{
  writeData("}", nullptr);
}

template <typename T>
void ASCIIJWriter::writeData(const char *f, T v)
{
  len += sprintf(buffer, bufferSize, f, v);
  myStream->write(buffer, len);
  myBytesWritten += len;
  flushBuffer();
}

void ASCIIJWriter::writeString(const char *data, size_t len)
{
  blen += sprintf(buf fer, "\"");
  for (size_t i = 0; i < len; i++)
  {
    // write the string
    if (blen > bufferSize - 2)
    {
      flushBuffer();
    }
    switch(data[i])
    {
      case "\"":
      	buffer[blen++] = "\\";
      	buffer[blen++] = "\"";
			case "\\":
				buffer[blen++] = "\\";
				buffer[blen++] = "\\";
			case "/":
				buffer[blen++] = "\\";
				buffer[blen++] = "/";
			case "\b":
				buffer[blen++] = "\\";
				buffer[blen++] = "b";
			case "\f":
				buffer[blen++] = "\\";
				buffer[blen++] = "f";
			case "\n":
				buffer[blen++] = "\\";
				buffer[blen++] = "n";
			case "\r":
				buffer[blen++] = "\\";
				buffer[blen++] = "r";
			case "\t":
				buffer[blen++] = "\\";
				buffer[blen++] = "t";
			default:
				buffer[blen++] = data[i];
    }
  }
}

void flushBuffer()
{
  if (myStream->write(buffer, blen))
  {
    myBytesWritten += blen;
    blen = 0;
  }
  else
  {
    ; // TODO: raise a flag here
  }
}
