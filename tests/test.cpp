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
void test_nonrepeat();
void test_format_float();
void test_format_mixfloat();


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
    { "format_nonrepeat", test_nonrepeat },
    { "format_float", test_format_float },
    { "format_mixfloat", test_format_mixfloat },
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

    format_i(cs, 10, 3, 3); // 010
    std::cout << cs << std::endl;

    format_i(cs, -10, 3, 3); // ***
    std::cout << cs << std::endl;

    format_i(cs, -1, 4, 3); // -001
    std::cout << cs << std::endl;

    format_i(cs, -1, 7, 3); // ___-001
    std::cout << cs << std::endl;

    format_d(cs, 1234.678, 9, 3); // 0.123D+04
    std::cout << cs << std::endl;

    format_d(cs, 1234.678, 8, 4); // ********
    std::cout << cs << std::endl;
    format_d(cs, 1234.678, 13, 4); // ___0.1235D+04
    std::cout << cs << std::endl;
    format_d(cs, -1234.678, 13, 4); // __-0.1235D+04
    std::cout << cs << std::endl;

    format_d(cs, -1234.678, 13, 5); // __-0.1235D+04
    std::cout << cs << std::endl;


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

    // fractional zeroes
    TEST_CHECK(frac_zeroes(2.0) == 0);
    TEST_CHECK(frac_zeroes(0.3) == 0);
    TEST_CHECK(frac_zeroes(0.02) == 1);
    TEST_CHECK(frac_zeroes(0.004) == 2);
    TEST_CHECK(frac_zeroes(-0.0005) == 3);
    TEST_CHECK(frac_zeroes(0.00001) == 4);
    TEST_CHECK(frac_zeroes(1.0E-08) == 9);
}


void test_integer()
{
    std::cout << '\n';
    std::ostringstream ss;

    printfor(ss, "(I5)", 5000);
    std::cout << ss.str() << std::endl;
    TEST_CHECK(compare_strings(ss.str().c_str(), " 5000"));
    ss.str(std::string());

    printfor(ss, "(I5)", 23);
    std::cout << ss.str() << std::endl;
    TEST_CHECK(compare_strings(ss.str().c_str(), "   23"));
    ss.str(std::string());

    printfor(ss, "(I5.3)", 23);
    std::cout << ss.str() << std::endl;
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

    // D and E
    printfor(ss, "(D9.3, 1X, D8.4, 1X, D13.4)", 1234.678, 1234.678 , 1234.678 );
    TEST_CHECK(compare_strings(ss.str().c_str(), "0.123D+04 ********    0.1235D+04"));
    std::cout << ss.str() << std::endl;
    ss.str(std::string());

    printfor(ss, "( E10.3, E11.4, E13.6 )", 12345678.0, 23.5678, 0.345678 );
    TEST_CHECK(compare_strings(ss.str().c_str(), " 0.123E+08 0.2357E+02 0.345678E+00"));
    std::cout << ss.str() << std::endl;
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

    format_f(cs, 0.45, width, 3);
    TEST_CHECK(compare_strings(cs, "0.450"));
    std::cout << cs << std::endl;
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
    std::cout << cs << std::endl;
    zerostr(cs);

    format_f(cs, -0.0, 3, 0);
    TEST_CHECK(compare_strings(cs, "-0."));
    std::cout << cs << std::endl;
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


void test_format_mixfloat()
{
    // for c string functions
    char cs[MAXLEN];
    zerostr(cs);

    // below 0.1 and without space
    format_g(cs, 0.01, 4, 3);
    TEST_CHECK(compare_strings(cs, "****"));
    zerostr(cs);

    // below 0.1 and without space
    format_g(cs, 0.01, 8, 3);
    TEST_CHECK(compare_strings(cs, ".100E-01"));
    zerostr(cs);

    // above 10**d
    format_g(cs, 1500., 8, 3);
    TEST_CHECK(compare_strings(cs, ".150E+04"));
    zerostr(cs);

    // several cases between 0.1 and 10**d
    format_g(cs, 1500., 9, 4);
    TEST_CHECK(compare_strings(cs, "1500.    "));
    zerostr(cs);

    // several cases between 0.1 and 10**d
    format_g(cs, 1500., 10, 4);
    TEST_CHECK(compare_strings(cs, " 1500.    "));
    zerostr(cs);

    // several cases between 0.1 and 10**d
    format_g(cs, 1500., 11, 4);
    TEST_CHECK(compare_strings(cs, "  1500.    "));
    zerostr(cs);

    format_g(cs, -1500., 11, 5);
    TEST_CHECK(compare_strings(cs, "-1500.0    "));
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