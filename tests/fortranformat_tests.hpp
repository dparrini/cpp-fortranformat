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


void printfor(char const*, ...);

void printfor(ostream&, char const*, ...);


struct Scanner {
    const char* start;
    const char* current;

    Scanner(const char* source)
    {
        this->start   = source;
        this->current = source;
    }
};


void stream_printfor(ostream&, char const*, va_list*);


void write_group(ostream&, Scanner*, va_list*);
void write_i(ostream&, Scanner*, va_list*, size_t repeat = 1);
void write_f(ostream&, Scanner*, va_list*, size_t repeat = 1);
void write_d(ostream&, Scanner*, va_list*, size_t repeat = 1);
void write_e(ostream&, Scanner*, va_list*, size_t repeat = 1);
void write_g(ostream&, Scanner*, va_list*, size_t repeat = 1);
void write_l(ostream&, Scanner*, va_list*, size_t repeat = 1);
void write_a(ostream&, Scanner*, va_list*, size_t repeat = 1);

void write_x(ostream&, Scanner*, size_t repeat = 1);
void write_str(ostream&, Scanner*);
void write_h(ostream&, Scanner*, size_t);

void fill_with_char(char*, char const, size_t const);
void format_i(char*, int, size_t, size_t);
void format_f(char*, double, size_t, size_t);
void format_d(char*, double, size_t, size_t, char expchar = 'D');
size_t fast_10pow(size_t const);
size_t integer_str_length(unsigned int);
size_t frac_zeroes(double);
void extract_integer_part(char*, double);
void extract_decimal_part(char*, double, size_t);

void consume(Scanner*);
void extract(Scanner*, char*, size_t);
inline char advance(Scanner*);
bool match(Scanner*, char);
inline char peek(Scanner*);
inline char peekNext(Scanner*);
bool isAtEnd(Scanner*);
inline bool isDigit(char);
inline bool isAlpha(char);
int integer(Scanner*);
void skipWhitespace(Scanner*);

#endif