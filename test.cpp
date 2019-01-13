#include <iostream>
#include "fortranformat.hpp"


int main()
{
	#ifdef DEBUG
	std::cout << "Internal tests" << std::endl;
	tests();
	std::cout << std::endl;
	#endif

	std::cout << "write() tests" << std::endl;
	write("(I5)", 5000);
	write("(I5)", 23);
	write("(I7.3)", 23);

	write("(F7.3)", 3.001345);
	write("(F7.0)", 3.001345);

	write("(L7)", true);
	write("(L7)", false);

	write("(A)", "test");
	write("(A8)", "abcdefg");
	write("(A3)", "abcdefg");
	// write("(I15, I15, I15)", 1, 0, 5);
	// write("(3I15)", 1, 0, 5);
}