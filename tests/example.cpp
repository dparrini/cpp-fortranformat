#include <fortranformat.hpp>


int main()
{
	printfor("(I3, 2(1X, F4.2))", 10, 3.1416, 3.333);

	return 0;
}