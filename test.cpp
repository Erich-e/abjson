#include "ASCIIJWriter.h"
#include <sstream>
#include <fstream>
#include <iostream>

using namespace abJSON;

int main(int argc, char *argv[])
{
	ASCIIJWriter test(&std::cout);
	test.null();
	test.null();
	test.null();
	test.null();
}