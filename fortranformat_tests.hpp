// C++ Fortran Format
// https://github.com/dparrini/cpp-fortranformat
// Copyright (c) 2019 David Parrini
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
 

#ifndef H_FORTRANFORMAT_TESTS__
#define H_FORTRANFORMAT_TESTS__

#include <cstdarg>
#include <ostream>
using std::ostream;


void printfor(char const* formatstr, ...);

void printfor(ostream& stream, char const* formatstr, ...);

void stream_printfor(ostream& stream, char const* formatstr, va_list* ap);


void tests();
struct Scanner {
    const char* start;
    const char* current;

    Scanner(const char* source)
    {
        this->start   = source;
        this->current = source;
    }
};


void write_group(ostream& stream, Scanner* scanner, va_list* ap);
void write_i(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_f(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_d(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_e(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_g(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_l(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_a(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat = 1);

void write_x(ostream& stream, Scanner* scanner, size_t repeat = 1);
void write_str(ostream& stream, Scanner* scanner);
void write_h(ostream& stream, Scanner* scanner, size_t length);

void format_f(char* put, double value, size_t width, size_t precision);
size_t fast_10pow(size_t const exponent);
size_t integer_str_length(unsigned int value);
void extract_integer_part(char* put, double value);
void extract_decimal_part(char* put, double value, size_t precision);

void consume(Scanner* scanner);
void extract(Scanner* scanner, char* put, size_t length);
inline char advance(Scanner* scanner);
bool match(Scanner* scanner, char expected);
inline char peek(Scanner* scanner);
inline char peekNext(Scanner* scanner);
bool isAtEnd(Scanner* scanner);
inline bool isDigit(char c);
inline bool isAlpha(char c);
int integer(Scanner* scanner);
void skipWhitespace(Scanner* scanner);

#endif