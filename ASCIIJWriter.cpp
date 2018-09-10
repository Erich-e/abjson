#include "ASCIIJWriter.h"
#include <inttypes.h>
#include <iostream>
#include <stdio.h>

using namespace abJSON;

ASCIIJWriter::ASCIIJWriter(std::ostream *baseStream, bool prettyPrint)
  : abstractJWriter(baseStream)
  , prettyPrint(prettyPrint)
  , blen(0)
{

}

ASCIIJWriter::~ASCIIJWriter()
{
}

void ASCIIJWriter::null()
{
  writeData("null", nullptr);
  myState.nextState(JOP::null);
  writeDelim();
}

void ASCIIJWriter::boolean(bool v)
{
  writeDelim();
  if (v)
  {
    writeData("true", nullptr);
  }
  else
  {
    writeData("false", nullptr);
  }
  myState.nextState(JOP::boolean);
}

void ASCIIJWriter::number(int32_t v)
{
  writeDelim();
  writeData<int32_t>("%" PRId32, v);
  myState.nextState(JOP::number);
}

void ASCIIJWriter::number(int64_t v)
{
  writeDelim();
  writeData<int64_t>("%" PRId64, v);
  myState.nextState(JOP::number);
}

void ASCIIJWriter::number(float v)
{
  writeDelim();
  writeData<float>("%g", v);
  myState.nextState(JOP::number);
}

void ASCIIJWriter::number(double v)
{
  writeDelim();
  writeData<double>("%g", v);
  myState.nextState(JOP::number);
}

void ASCIIJWriter::key(const char *v, int64_t len)
{
  writeDelim();
  writeString(v, len);
  myState.nextState(JOP::key);
}

void ASCIIJWriter::string(const char *v, int64_t len)
{
  writeDelim();
  writeString(v, len);
  myState.nextState(JOP::string);
}

void ASCIIJWriter::beginArray()
{
  writeDelim();
  writeData("[", nullptr);
  myState.nextState(JOP::beginArray);
}

void ASCIIJWriter::endArray()
{
  writeDelim();
  writeData("]", nullptr);
  myState.nextState(JOP::endArray);
}

void ASCIIJWriter::beginMap()
{
  writeDelim();
  writeData("{", nullptr);
  myState.nextState(JOP::beginMap);
}

void ASCIIJWriter::endMap()
{
  writeDelim();
  writeData("}", nullptr);
  myState.nextState(JOP::endMap);
}

template <typename T>
void ASCIIJWriter::writeData(const char *f, T v)
{
  // use pointer arithmetic to avoid overiting the buffer
  blen += snprintf(buffer+blen, bufferSize, f, v);
  flushBuffer();
}

void ASCIIJWriter::writeString(const char *data, size_t len)
{
  blen += sprintf(buffer, "\"");
  for (size_t i = 0; i < len; i++)
  {
    // write the string
    if (blen > bufferSize - 2)
    {
      flushBuffer();
    }
    switch(data[i])
    {
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

}

void ASCIIJWriter::flushBuffer()
{
  if (myStream->write(buffer, blen))
  {
    myBytesWritten += blen;
    blen = 0;
  }
  else
  {
    std::cout << "something went wrong" << std::endl;
    ; // TODO: raise a flag here
  }
}

void ASCIIJWriter::writeDelim()
{
  switch (myState.getState())
  {
    // perform a switch on the CURRENT state.
    case JState::array:
    case JState::mapValue:
    // write a comma
      blen += snprintf(buffer+blen, bufferSize, ",");
      break;
    case JState::mapKey:
    // write a colon
      blen += snprintf(buffer+blen, bufferSize, ":");
      break;
  }
}