#include <iostream>
#include "fortranformat_tests.hpp"
#include "tests/acutest.h"


void test_basic(void);


TEST_LIST = {
	{ "basic", test_basic },
	{0}
};


void test_basic(void)
{
	#ifdef DEBUG
	std::cout << "Internal tests" << std::endl;
	tests();
	std::cout << std::endl;
	#endif

	std::cout << "printfor() tests" << std::endl;
	printfor("(I5)", 5000);
	printfor("(I5)", 23);
	printfor("(I7.3)", 23);

	printfor("(F7.3)", 3.001345);
	printfor("(F7.0)", 3.001345);

	printfor("(L7)", true);
	printfor("(L7)", false);

	printfor("(A)", "test");
	printfor("(A8)", "abcdefg");
	printfor("(A3)", "abcdefg");

	printfor("(3I3)", 1, 2, 3);
	printfor("  (((  3I3  )))  ", 1, 2, 3);
	printfor("(3I3) ignored", 1, 2, 3);
	printfor("(3A10)", "test1", "test2", "test3");
	printfor("(I15, I15, I15)", 1, 0, 5);
	printfor("(A, 2(I15, I3))", "test", 1, 0, 5, 7);
	printfor("(3(I3)) ignored", 1, 2, 3);
	printfor("(3(I3, 3X))", 1, 2, 3);
	printfor("(A, 'test')", "different ");
	printfor("('test', A)", " different");
	printfor("(5Htest , A)", "different");

	std::cout << "F ";
	printfor("(F10.3)", 3.001345);
	std::cout << "D ";
	printfor("(D10.3)", 3.001345);
	std::cout << "E ";
	printfor("(E10.3)", 3.001345);
	std::cout << "G ";
	printfor("(G10.3)", 3.001345);

	printfor("(F10.3)", -3.001345);

	TEST_CHECK(true);
}