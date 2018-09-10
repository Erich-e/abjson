#ifndef ABJSON_ASCIIJWRITER
#define ABJSON_ASCIIJWRITER
#include "abstractJWriter.h"

namespace abJSON
{
  class ASCIIJWriter : public abstractJWriter
  {
  private:
    static const int bufferSize = 256;

    char          buffer[bufferSize];
    int           blen;
    bool          prettyPrint;

    bool          error;

  public:
    ASCIIJWriter(std::ostream *baseStream, bool prettyPrint = true);
    ~ASCIIJWriter();

    /// abstractJWriter overrides
    void null() override;
    void boolean(bool v) override;
    void number(int32_t v) override;
    void number(int64_t v) override;
    void number(float v) override;
    void number(double v) override;
    void key(const char *v, int64_t len=-1) override;
    void string(const char *v, int64_t len=-1) override;

    void beginArray() override;
    void endArray() override;
    void beginMap() override;
    void endMap() override;

  private:
    /// Formats the data against f and writes to output.
    template <typename T>
    void writeData(const char *f, T v);
    /// excapes and writes a string
    void writeString(const char *data, size_t len);
    /// flush buffer - used internally
    void flushBuffer();
    /// write a delimeter, based on the current state
    void writeDelim();
  };
}
#endif