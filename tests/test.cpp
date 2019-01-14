#include <cstring>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include "fortranformat_tests.hpp"
#include "acutest.h"


void test_basic();
void test_integer();
void test_float();
void test_logical();
void test_repeat();
void test_alpha();
void test_combined();
void test_doubles();
void test_nonrepeat();
void test_format_float();


void trim_lb(char const* source, char* dest);
void zerostr(char* str);

size_t const MAXLEN = 200;


bool compare_strings(char const* str1, char const* str2)
{
    // std::cout << str1 << '\n' << str2 << '\n';
    char nstr1[MAXLEN];
    trim_lb(str1, nstr1);
    char nstr2[MAXLEN];
    trim_lb(str2, nstr2);

    return strcmp(nstr1, nstr2) == 0;
}


TEST_LIST = {
    { "basic", test_basic },
    { "integer", test_integer },
    { "float", test_float },
    { "logical", test_logical },
    { "alpha", test_alpha },
    { "repeat", test_repeat },
    { "combined", test_combined },
    { "doubles", test_doubles },
    { "format_nonrepeat", test_nonrepeat },
    { "format_float", test_format_float },
    {0}
};


void test_basic()
{
    std::cout << '\n';
    // for stream functions
    std::ostringstream ss;
    // for c string functions
    char cs[MAXLEN];
    zerostr(cs);

    // fractional part extraction
    extract_decimal_part(cs, 3.14, 2);
    TEST_CHECK(compare_strings(cs, "14"));
    zerostr(cs);

    extract_decimal_part(cs, 3.00, 2);
    TEST_CHECK(compare_strings(cs, "00"));
    zerostr(cs);

    extract_decimal_part(cs,  4.00002, 5);
    TEST_CHECK(compare_strings(cs, "00002"));
    zerostr(cs);

    // integer character length
    TEST_CHECK(integer_str_length(0) == 1);
    TEST_CHECK(integer_str_length(11) == 2);
    TEST_CHECK(integer_str_length(100) == 3);
    TEST_CHECK(integer_str_length(1063) == 4);
    TEST_CHECK(integer_str_length(100000003) == 9);

    // integer parts
    extract_integer_part(cs, 0.45);
    TEST_CHECK(compare_strings(cs, "0"));
    zerostr(cs);

    extract_integer_part(cs, 11.3);
    TEST_CHECK(compare_strings(cs, "11"));
    zerostr(cs);

    extract_integer_part(cs, 100.7003);
    TEST_CHECK(compare_strings(cs, "100"));
    zerostr(cs);

    extract_integer_part(cs, 1063.0);
    TEST_CHECK(compare_strings(cs, "1063"));
    zerostr(cs);

    extract_integer_part(cs, 100000003.0004);
    TEST_CHECK(compare_strings(cs, "100000003"));
    zerostr(cs);
}


void test_integer()
{
    std::cout << '\n';
    std::ostringstream ss;

    printfor(ss, "(I5)", 5000);
    TEST_CHECK(compare_strings(ss.str().c_str(), " 5000"));
    ss.str(std::string());

    printfor(ss, "(I5)", 23);
    TEST_CHECK(compare_strings(ss.str().c_str(), "   23"));
    ss.str(std::string());

    printfor(ss, "(I5.3)", 23);
    TEST_CHECK(compare_strings(ss.str().c_str(), "  023"));
    ss.str(std::string());
}


void test_float()
{
    std::cout << '\n';
    std::ostringstream ss;

    printfor(ss, "(F7.3)", 3.001345);
    TEST_CHECK(compare_strings(ss.str().c_str(), "  3.001"));
    ss.str(std::string());

    printfor(ss, "(F7.0)", 3.001345);
    TEST_CHECK(compare_strings(ss.str().c_str(), "     3."));
    ss.str(std::string());

    printfor(ss, "(F7.3, 1X, F5.3, 1X, F4.3)", 3.001345, 3.001345, 3.001345);
    TEST_CHECK(compare_strings(ss.str().c_str(), "  3.001 3.001 ****"));
    ss.str(std::string());
}


void test_logical()
{
    std::cout << '\n';
    std::ostringstream ss;

    printfor(ss, "(L7)", true);
    TEST_CHECK(compare_strings(ss.str().c_str(), "      T"));
    ss.str(std::string());

    printfor(ss, "(L7)", false);
    TEST_CHECK(compare_strings(ss.str().c_str(), "      F"));
    ss.str(std::string());
}


void test_repeat()
{
    std::cout << '\n';
    std::ostringstream ss;

    printfor(ss, "(3I3)", 1, 2, 3);
    TEST_CHECK(compare_strings(ss.str().c_str(), "  1  2  3"));
    ss.str(std::string());

    printfor(ss, "(3A10)", "test1", "test2", "test3");
    TEST_CHECK(compare_strings(ss.str().c_str(), "     test1     test2     test3"));
    ss.str(std::string());

    printfor(ss, "(3(I3, 3X))", 1, 2, 3);
    TEST_CHECK(compare_strings(ss.str().c_str(),"  1     2     3"));
    ss.str(std::string());

    ss.str(std::string());
}

void test_alpha()
{
    std::cout << '\n';
    std::ostringstream ss;

    printfor(ss, "(A)", "test");
    TEST_CHECK(compare_strings(ss.str().c_str(), "test"));
    ss.str(std::string());

    printfor(ss, "(A8)", "abcdefg");
    TEST_CHECK(compare_strings(ss.str().c_str(), " abcdefg"));
    ss.str(std::string());

    printfor(ss, "(A3)", "abcdefg");
    TEST_CHECK(compare_strings(ss.str().c_str(), "abc"));
    ss.str(std::string());

    printfor(ss, "(A, 'test')", "different ");
    TEST_CHECK(compare_strings(ss.str().c_str(), "different test"));
    ss.str(std::string());

    printfor(ss, "('test', A)", " different");
    TEST_CHECK(compare_strings(ss.str().c_str(), "test different"));
    ss.str(std::string());

    printfor(ss, "(5Htest , A)", "different");
    TEST_CHECK(compare_strings(ss.str().c_str(), "test different"));
    ss.str(std::string());
}


void test_combined()
{
    std::cout << '\n';
    std::ostringstream ss;

    printfor(ss, "  (((  3I3  )))", 1, 2, 3);
    TEST_CHECK(compare_strings(ss.str().c_str(), "  1  2  3"));
    ss.str(std::string());

    printfor(ss, "(3I3) ignored", 1, 2, 3);
    TEST_CHECK(compare_strings(ss.str().c_str(), "  1  2  3"));
    ss.str(std::string());

    printfor(ss, "(A, 2(I15, I3))", "test", 1, 0, 5, 7);
    TEST_CHECK(compare_strings(ss.str().c_str(), "test              1  0              5  7"));
    ss.str(std::string());

    printfor(ss, "(3(I3)) ignored", 1, 2, 3);
    TEST_CHECK(compare_strings(ss.str().c_str(), "  1  2  3"));
    ss.str(std::string());

    // it seems that the Fortran implementation doesn't insert the last 3X whitespaces.
    printfor(ss, "(3(I3, 3X))", 1, 2, 3);
    TEST_CHECK(compare_strings(ss.str().c_str(), "  1     2     3   "));
    ss.str(std::string());
}


void test_doubles()
{
    std::cout << '\n';
    std::cout << "F ";
    printfor("(F10.3)", 3.001345);
    std::cout << "D ";
    printfor("(D10.3)", 3.001345);
    std::cout << "E ";
    printfor("(E10.3)", 3.001345);
    std::cout << "G ";
    printfor("(G10.3)", 3.001345);

    printfor("(F10.3)", -3.001345);

    TEST_CHECK(false);
}

void test_nonrepeat()
{
    std::cout << '\n';
    std::ostringstream ss;

    // unspecified X count and no arguments
    printfor(ss, "('test', X, 'test')");
    TEST_CHECK(compare_strings(ss.str().c_str(), "test test"));
    ss.str(std::string());

    // specified X count and no arguments
    printfor(ss, "('test', 5X, 'test')");
    TEST_CHECK(compare_strings(ss.str().c_str(), "test     test"));
    ss.str(std::string());

    // test / line feed
    printfor(ss, "('test' / 'test')");
    TEST_CHECK(compare_strings(ss.str().c_str(), "test\ntest"));
    ss.str(std::string());
}


void test_format_float()
{
    // for c string functions
    char cs[MAXLEN];
    zerostr(cs);

    // write
    size_t width = 5;

    format_f(cs, 0.0, 1, 0);
    TEST_CHECK(compare_strings(cs, "*"));
    zerostr(cs);
    std::cout << cs << std::endl;

    format_f(cs, 0.45, width, 3);
    TEST_CHECK(compare_strings(cs, "0.450"));
    zerostr(cs);

    format_f(cs, 11.3, width, 3);
    TEST_CHECK(compare_strings(cs, "*****"));
    zerostr(cs);

    format_f(cs, 100.7003, width, 1);
    TEST_CHECK(compare_strings(cs, "100.7"));
    zerostr(cs);

    format_f(cs, 0.0235, width, 4);
    TEST_CHECK(compare_strings(cs, ".0235"));
    zerostr(cs);

    format_f(cs, 100000003.0004, 10, 0);
    TEST_CHECK(compare_strings(cs, "100000003."));
    zerostr(cs);

    // write
    format_f(cs, -0.0, 1, 0);
    TEST_CHECK(compare_strings(cs, "*"));
    zerostr(cs);

    format_f(cs, -0.0, 2, 0);
    TEST_CHECK(compare_strings(cs, "**"));
    zerostr(cs);

    format_f(cs, -0.0, 3, 0);
    TEST_CHECK(compare_strings(cs, "-0."));
    zerostr(cs);

    format_f(cs, -0.45, width-1, 3);
    TEST_CHECK(compare_strings(cs, "****"));
    zerostr(cs);

    format_f(cs, -0.45, width, 3);
    TEST_CHECK(compare_strings(cs, "-.450"));
    zerostr(cs);

    format_f(cs, -0.45, width+1, 3);
    TEST_CHECK(compare_strings(cs, "-0.450"));
    zerostr(cs);

    format_f(cs, -11.3, width+1, 3);
    TEST_CHECK(compare_strings(cs, "******"));
    zerostr(cs);

    format_f(cs, -100.7003, width+1, 1);
    TEST_CHECK(compare_strings(cs, "-100.7"));
    zerostr(cs);

    format_f(cs, -0.0235, width+1, 4);
    TEST_CHECK(compare_strings(cs, "-.0235"));
    zerostr(cs);

    format_f(cs, -100000003.0004, 11, 0);
    TEST_CHECK(compare_strings(cs, "-100000003."));
    zerostr(cs);
}


void trim_lb(char const* source, char* dest)
{
    size_t len = strlen(source);
    size_t pos_src = 0;
    size_t pos_dest = 0;

    // look for the end index
    size_t last = len - 1;
    char c = source[last];

    while (std::isspace(c) && last > 0)
    {
        last = last - 1;
        c = source[last];
    }

    while (pos_src <= last)
    {
        if (source[pos_src] != '\n' || source[pos_src] != '\r' )
        {
            dest[pos_dest] = source[pos_src];
            pos_dest = pos_dest + 1;
        }
        else
        {
            break;
        }
        pos_src = pos_src + 1;
    }
    dest[pos_dest] = '\0';
}


void zerostr(char* str)
{
    size_t len = MAXLEN;
    for (size_t pos = 0; pos < len; ++pos)
    {
        str[pos] = ' ';
    }
    str[len-1] = '\0';
}