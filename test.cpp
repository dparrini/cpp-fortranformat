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

	write("(3I3)", 1, 2, 3);
	write("  (((  3I3  )))  ", 1, 2, 3);
	write("(3I3) ignored", 1, 2, 3);
	write("(3A10)", "test1", "test2", "test3");
	write("(I15, I15, I15)", 1, 0, 5);
	write("(A, 2(I15, I3))", "test", 1, 0, 5, 7);
	write("(3(I3)) ignored", 1, 2, 3);
	write("(3(I3, 3X))", 1, 2, 3);
	write("(A, 'test')", "different ");
	write("('test', A)", " different");
	write("(5Htest , A)", "different");
	
	std::cout << "F ";
	write("(F10.3)", 3.001345);
	std::cout << "D ";
	write("(D10.3)", 3.001345);
	std::cout << "E ";
	write("(E10.3)", 3.001345);
	std::cout << "G ";
	write("(G10.3)", 3.001345);
}