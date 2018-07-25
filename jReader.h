// #include "jDefines.h"
// #include <map>

// namespace abJSON
// {
// 	template <typename STREAM_T, typename VERB_T, bool BINARY=false>
// 	class jReader
// 	{
// 	public:
// 		using real32_t = float;
// 		using real64_t = double;
// 		enum errorState {
// 			NO_ERROR = 0,
// 			STREAM_ERROR,
// 			BAD_INPUT,
// 			END_OF_FILE
// 		};

// 		jReader(STREAM_T *stream=nullptr, VERB_T *verb=nullptr)
// 			: myStream(stream)
// 			, myVerb(verb)
// 			, myErrorState(NO_ERROR)
// 			, uniform(false)
// 		{}
// 		void readNext()
// 		{
// 			if (*this) {
// 				if (BINARY)
// 				{
// 					char t = myStream->peek();
// 					switch (t) {
// 						case JTYPE::JNULL:
// 							readNull();
// 							break;
// 						case JTYPE::JBOOL:
// 							myStream->get();
// 							char b = myStream->peek();
// 							if (b == 1)
// 							{
// 								myVerb->takeBool(true);
// 							}
// 							else if (b == 0)
// 							{
// 								myVerb->takebool(true);
// 							}
// 							else
// 							{
// 								// error
// 							}
// 							break;
// 						case JTYPE::JTRUE:
// 							myStream->get();
// 							myVerb->takeBool(true);
// 						case JTYPE::JFALSE:
// 							myStream->get();
// 							myVerb->takeBool(false);
// 						case JTYPE::JSTRING:
// 							readString();
// 							break;
// 						case JTYPE::JTOKENDEF:
// 							// defString();
// 							break;
// 						case JTYPE::JTOKENREF:
// 							// refString();
// 							break;
// 						case JTYPE::JTOKENUNDEF:
// 							// undefString();
// 						case JTYPE::JARRAY_BEGIN:
// 							readArrayBegin();
// 							break;
// 						case JTYPE::JARRAY_END:
// 							readArrayEnd();
// 							break;
// 						case JTYPE::JUNIFORM_ARRAY:
// 							startUniformArray();
// 						case JTYPE::JMAP_BEGIN:
// 							readMapBegin();
// 							break;
// 						case JTYPE::JMAP_END:
// 							readMapEnd();
// 							break;
// 					}
// 				}
// 				else
// 				{
// 					char c = myStream->peek();
// 					switch (c)
// 					{
// 						case ' ':
// 						case '\t':
// 						case '\r':
// 						case '\n':
// 						case '\v':
// 						case '\f':
// 							myStream->get();
// 							readNext();
// 							break;
// 						case 'n':
// 							readNull();
// 							break;
// 						case 't':
// 						case 'f':
// 							readBool();
// 							break;
// 						case '\"':
// 							readString();
// 							break;
// 						case '[':
// 							readArrayBegin();
// 							break;
// 						case ']':
// 							readArrayEnd();
// 							break;
// 						case '{':
// 							readMapBegin();
// 							break;
// 						case '}':
// 							readMapEnd();
// 							break;
// 						case '-':
// 							myStream->get();
// 							readASCIINumber(true);
// 						case EOF:
// 							// end of file
// 							myStream->get();
// 							myErrorState = EOF;
// 						default:
// 							if (47 < c && c < 58)
// 							{
// 								readASCIINumber(false);
// 							}
// 							else
// 							{
// 								;// error
// 							}
// 					}
// 					readDelim();
// 				}
// 			}
// 		}

// 		/// State Functions:
// 		errorState getErrorState() {
// 			return myErrorState;
// 		}

// 		bool fail() {
// 			return myErrorState != NO_ERROR;
// 		}

// 		bool eof() {
// 			return myErrorState == EOF;
// 		}

// 		void clear() {
// 			myErrorState == NO_ERROR;
// 		}

// 		explicit operator bool(){
// 			return myErrorState == NO_ERROR;
// 		}

// 	private:
// 		STREAM_T	*myStream;
// 		VERB_T		*myVerb;

// 		errorState myErrorState;

// 		// BINARY
// 		bool 						uniform;
// 		std::map<int, std::string>	strTokens;

// 		void readNull()
// 		{
// 			if (BINARY)
// 			{
// 				;
// 			}
// 			else
// 			{
// 				char t1 = myStream->get();
// 				if (t1 == 'n')
// 				{
// 					char t2 = myStream->get();
// 					if (t2 == 'u')
// 					{
// 						char t3 = myStream->get();
// 						if (t3 == 'l')
// 						{
// 							char t4 = myStream->get();
// 							if (t4 == 'l')
// 							{
// 								myVerb->takeNull();// Pass on
// 								return;
// 							}
// 							myStream->putback(t4);
// 						}
// 						myStream->putback(t3);
// 					}
// 					myStream->putback(t2);
// 				}
// 				myStream->putback(t1);
// 				myErrorState = BAD_INPUT;
// 			}
// 		}

// 		void readBool()
// 		{
// 			if (BINARY)
// 			{
// 				;
// 			}
// 			else
// 			{
// 				char t1 = myStream->get();
// 				if (t1 == 't')
// 				{
// 					char t2 = myStream->get();
// 					if (t2 == 'r')
// 					{
// 						char t3 = myStream->get();
// 						if (t3 == 'u')
// 						{
// 							char t4 = myStream->get();
// 							if (t4 == 'e')
// 							{
// 								myVerb->takeBool(true);
// 								return;
// 							}
// 							myStream->putback(t4);
// 						}
// 						myStream->putback(t3);
// 					}
// 					myStream->putback(t2);
// 				}
// 				else if (t1 == 'f')
// 				{
// 					char t2 = myStream->get();
// 					if (t2 == 'a')
// 					{
// 						char t3 = myStream->get();
// 						if (t3 == 'l')
// 						{
// 							char t4 = myStream->get();
// 							if (t4 == 's')
// 							{
// 								char t5 = myStream->get();
// 								if (t5 == 'e')
// 								{
// 									myVerb->takeBool(false);
// 									return;
// 								}
// 								myStream->putback(t5);
// 							}
// 							myStream->putback(t4);
// 						}
// 						myStream->putback(t3);
// 					}
// 					myStream->putback(t2);
// 				}
// 				myStream->putback(t1);
// 				myErrorState = BAD_INPUT;
// 			}
// 		}

// 		void readString() {
// 			if (BINARY)
// 			{
// 				;
// 			}
// 			else
// 			{
// 				int capacity = 128;
// 				int blen = 0;
// 				char *buffer = malloc(sizeof(char)*capacity);
// 				char quote = myStream->get();

// 				char c = myStream->get();
// 				while (c != '\"')
// 				{
// 					if (blen == capacity - 1);
// 					{
// 						capacity *= 2;
// 						buffer = realloc(buffer, sizeof(char)*capacity);
// 					}
// 					if (c == '\\')
// 					{
// 						c = myStream->get();
// 						switch(c)
// 						{
// 							case '\"':
// 								buffer[blen++] = '\"';
// 								break;
// 							case '\\':
// 								buffer[blen++] = '\\';
// 								break;
// 							case '/':
// 								buffer[blen++] = '/';
// 								break;
// 							case 'b':
// 								buffer[blen++] = '\b';
// 								break;
// 							case 'f':
// 								buffer[blen++] = '\f';
// 								break;
// 							case 'n':
// 								buffer[blen++] = '\n';
// 								break; 
// 							case 'r':
// 								buffer[blen++] = '\r';
// 								break;
// 							case 't':
// 								buffer[blen++] = '\t';
// 								break;
// 							default:
// 								for (int i = blen-1; i >= 0; i--)
// 								{
// 									myStream->putback(buffer[i]);
// 								}
// 								myErrorState = BAD_INPUT;
// 								return;
// 						}
// 					}
// 					else
// 					{
// 						switch(c)
// 						{
// 							case '/':
// 							case '\b':
// 							case '\f':
// 							case '\n':
// 							case '\r':
// 							case '\t':
// 							case '\0':
// 								for (int i = blen - 1; i >= 0; i--)
// 								{
// 									myStream->putback(buffer[i]);
// 								}
// 								myErrorState = BAD_INPUT;
// 							default:
// 								buffer[blen++] = c;
// 						}
// 						buffer[blen++] = c;
// 					}
// 					c = myStream->get();
// 				}
// 				buffer[blen++] = '\0';
// 				myVerb->takeString(buffer, blen);
// 				free(buffer);
// 			}	
// 		}

// 		void readArrayBegin()
// 		{
// 			myStream->get();
// 			myVerb->takeArrayBegin();
// 		}

// 		void readArrayEnd()
// 		{
// 			myStream->get();
// 			myVerb->takeArrayEnd();
// 		}

// 		void readMapBegin()
// 		{
// 			myStream->get();
// 			myVerb->takeMapBegin();
// 		}
// 		void readMapEnd()
// 		{
// 			myStream->get();
// 			myVerb->takeMapEnd();
// 		}

// 		template <typename T>
// 		void readNumber()
// 		{
// 			// Binary is assumed
// 			T val = 0;
// 		}

// 		void readASCIINumber(bool negative)
// 		{
// 			// ASCII is assumed
// 			;
// 		}

// 		void readLength()
// 		{
// 			;
// 		}

// 		void readDelim()
// 		{
// 			;
// 		}

// 		void startUniformArray()
// 		{
// 			;
// 		}
// 	};
// }