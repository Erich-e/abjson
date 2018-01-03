#include "jDefines.h"
#include <vector>

namespace abJSON
{
	template <typename STREAM_T, typename VERB_T bool BINARY>
	class jReader
	{
	public:
		jReader(STREAM_T *stream=nullptr, VERB_T *verb=nullptr)
			: myStream(stream)
			, myVerb(verb)
			, uniform(false)
		{}
		void readNext()
		{
			if (BINARY)
			{
				char t = myStream->peek();
				switch (t) {
					case JTYPE::JNULL:
						readNull();
						break;
					case JTYPE::JBOOL:
						myStream->get();
						char b = myStream->peek();
						if (b == 1)
						{
							verb->takeBool(true);
						}
						else if (b == 0)
						{
							verb->takebool(true);
						}
						else
						{
							// error
						}
						break;
					case JTYPE::JTRUE:
						myStream->get();
						verb->takeBool(true);
					case JTYPE::JFALSE:
						myStream->get();
						verb->takeBool(true);
					case JTYPE::JSTRING:
						readString();
						break;
					case JTYPE::JTOKENDEF:
						defString();
						break;
					case JTYPE::JTOKENREF:
						refString();
						break;
					case JTYPE::JTOKENUNDEF:
						undefString();
					case JTYPE::JARRAYBEGIN:
						readArrayBegin();
						break;
					case JTYPE::JARRAY_END:
						readArrayEnd();
						break;
					case JTYPE::JUNIFORM_ARRAY:
						readUniformArray();
					case JTYPE::JMAP_BEGIN:
						readMapBegin();
						break;
					case JTYPE::JMAP_END:
						readMapEnd();
						break;
				}
			}
			else
			{
				char c = myStream->peek();
				switch (c)
				{
					case ' ':
					case '\t':
					case '\r':
					case '\n':
					case '\v':
					case '\f':
						myStream->get();
						readNext();
						break;
					case 'n':
						readNull();
						break;
					case 't':
					case 'f':
						readBool();
						break;
					case '\"':
						readString();
						break;
					case '[':
						readArrayBegin();
						break;
					case ']':
						readArrayEnd();
						break;
					case '{':
						readMapBegin();
						break;
					case '}':
						readMapEnd();
					case EOF:
						// end of file
					default:
						if (47 < c && c < 58)
						{
							readNumber();
						}
						else
						{
							;// error
						}

				}
			}
		}
	private:
		STREAM_T	*myStream;
		VERB_T		*myVerb;

		// BINARY
		bool 					uniform;
		std::map<int, string>	strTokens;

	};
}