#include "abstractJWriter.h"

using namespace abJSON;

abstractJWriter::abstractJWriter(std::ostream *baseStream)
    : myStream(baseStream)
    , myBytesWritten(0)
{
}

abstractJWriter::~abstractJWriter()
{}

size_t abstractJWriter::bytesWritten()
{
    return myBytesWritten;
}