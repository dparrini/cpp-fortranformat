#include <fortranformat.hpp>


int main()
{
	printfor("(I3, SP, 2(1X, F5.2), 2X, SS, G9.3E3)", 
        10, 3.1416, 3.333, 2.7545E-12);

	return 0;
}