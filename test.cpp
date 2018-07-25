#include "jWriter.h"
#include <sstream>
#include <fstream>
#include <iostream>

using namespace abJSON;

// int main(int argc, char* argv[])
// {
// 	//std::ostringstream					os;
// 	std::fstream						f("tmp");
// 	jWriter<std::ostream, true>			w(&f);

// 	// w.beginArray();
// 	// w.number(5);
// 	// w.beginArray();
// 	// w.number(1);
// 	// w.number(2);
// 	// w.number(3);
// 	// w.endArray();
// 	// w.beginMap();
// 	// w.string("key");
// 	// w.string("value");
// 	// w.endMap();
// 	// w.endArray();

// 	// w.beginMap();
// 	// w.string("key");
// 	// w.string("value");
// 	// w.string("key2");
// 	// w.string("value2");
// 	// w.string("array");
// 	// w.beginArray();
// 	// w.number(1);
// 	// w.number(-1);
// 	// w.number(100);
// 	// w.number(10000);
// 	// w.endArray();
// 	// w.endMap();

// 	w.number(1);

// 	if(w) {
// 		std::cout << "no error\n";
// 	}
// 	// else if (w.getErrorState() == 1) {
// 	// 	std::cout << "stream error\n";
// 	// }
// 	// else if (w.getErrorState() == 2) {
// 	// 	std::cout << "bad input\n";
// 	// }
// }

int main(int argc, char *argv[])
{
	jWriter<true> test(&std::cout);
	test.null();
	test.null();
	test.null();
}