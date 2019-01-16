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


#include <cmath>
#include <cstdarg>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <ostream>
#include "fortranformat.hpp"
using std::ostream;

size_t const MAX_STR_LEN = 200;

#ifndef DEBUG

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
void write_nl(ostream&, Scanner*);
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


void printfor(char const* formatstr, ...)
{
    va_list ap;
    va_start(ap, formatstr);
    stream_printfor(std::cout, formatstr, &ap);
    va_end(ap);
}


void printfor(ostream& stream, char const* formatstr, ...)
{
    va_list ap;
    va_start(ap, formatstr);
    stream_printfor(stream, formatstr, &ap);
    va_end(ap);
}


void stream_printfor(ostream& stream, char const* formatstr, va_list* ap)
{
    Scanner scanner(formatstr);
    skipWhitespace(&scanner);
    char c = advance(&scanner);
    if ('(' == c)
    {
        write_group(stream, &scanner, ap);
    }
    stream << '\n';    
}


void write_group(ostream& stream, Scanner* scanner, va_list* ap)
{
    // consume open paren and following whitespace
    skipWhitespace(scanner);
    consume(scanner);

    for (;;)
    {
        char c = advance(scanner);

        // stream << "scanning " << c << '\n';

        // repeat count
        unsigned int repeat = 1;
        if (isDigit(c))
        {
            repeat = integer(scanner);
            // TODO: repeat must be nonzero
            c = advance(scanner);
        }

        // nested group
        if ('(' == c)
        {
            char const* previous = scanner->current;
            for (size_t repcount = 0; repcount < repeat; ++repcount)
            {
                scanner->start = previous;
                scanner->current = previous;
                write_group(stream, scanner, ap);    
            }
        }
        // edit descriptors
        else if (isAlpha(c))
        {
            switch(c)
            {
                case 'I':
                    write_i(stream, scanner, ap, repeat); 
                break;

                case 'F':
                    write_f(stream, scanner, ap, repeat); 
                break;

                case 'D':
                    write_d(stream, scanner, ap, repeat); 
                break;

                case 'E':
                    write_e(stream, scanner, ap, repeat); 
                break;

                case 'G':
                    write_g(stream, scanner, ap, repeat); 
                break;

                case 'L':
                    write_l(stream, scanner, ap, repeat); 
                break;

                case 'A':
                    write_a(stream, scanner, ap, repeat); 
                break;

                case 'X':
                    write_x(stream, scanner, repeat); 
                break;

                case 'H':
                    write_h(stream, scanner, repeat); 
                break;
            }
        }
        else if ('/' == c)
        {
            write_nl(stream, scanner);
        }
        else if ('\'' == c)
        {
            write_str(stream, scanner);
        }
        else if (')' == c)
        {
            consume(scanner);
            break;
        }
        else if (',' == c)
        {
            consume(scanner);
        }

        if (isAtEnd(scanner))
        {
            break;
        }

        skipWhitespace(scanner);
        consume(scanner);
    }
}


//
// Format write edit descriptors
//

void write_i(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat)
{
    consume(scanner);

    int width = integer(scanner);
    int fill = 0;
    
    if (peek(scanner) == '.')
    {
        advance(scanner);
        consume(scanner);
        fill = integer(scanner);
    }

    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        int value = va_arg(*ap, int); 
        char put[MAX_STR_LEN];

        format_i(put, value, width, fill);
        std::cout << put;
    }
}


void write_f(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat)
{
    consume(scanner);

    int width = integer(scanner);
    int decimal = 0;

    // dot character
    advance(scanner); 
    consume(scanner);
    decimal = integer(scanner);
    
    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        double value = va_arg(*ap, double); 
        char put[MAX_STR_LEN];

        format_f(put, value, width, decimal);  
        std::ios_base::fmtflags f(stream.flags());
        stream << std::setw(width) << std::right << put;
        stream.flags(f);  
    }
}


void write_d(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat)
{
    // TODO: similar to write_e
    consume(scanner);

    int width = integer(scanner);
    int decimal = 0;

    // dot character
    advance(scanner); 
    consume(scanner);
    decimal = integer(scanner);
    
    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        double value = va_arg(*ap, double); 
        char put[MAX_STR_LEN];

        format_d(put, value, width, decimal, 'D');  
        stream << put;
    }
}


void write_e(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat)
{
    // TODO: similar to write_d
    consume(scanner);

    int width = integer(scanner);
    int decimal = 0;

    // dot character
    advance(scanner); 
    consume(scanner);
    decimal = integer(scanner);
    
    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        double value = va_arg(*ap, double); 
        char put[MAX_STR_LEN];

        format_d(put, value, width, decimal, 'E');  
        stream << put;
    }
}


void write_g(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat)
{
    // partial support
    write_f(stream, scanner, ap, repeat);
}


void write_l(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat)
{
    consume(scanner);
    int width = integer(scanner);

    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        int value = va_arg(*ap, int); 
        char valstr;
        if (value)
        {
            valstr = 'T';
        }
        else
        {
            valstr = 'F';   
        }
        std::ios_base::fmtflags f(stream.flags());
        stream << std::right << std::setw(width) << valstr;
        stream.flags(f);
    }
}


void write_a(ostream& stream, Scanner* scanner, va_list* ap, size_t repeat)
{
    consume(scanner);
    int width = 0;
    if (isDigit(peek(scanner)))
    {
        width = integer(scanner);
    }

    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        char const* value = va_arg(*ap, char const*); 

        std::ios_base::fmtflags f(stream.flags());
        if (width > 0)
        {
            char valsub[MAX_STR_LEN];
            strncpy(valsub, value, width);
            valsub[width] = '\0';

            stream << std::setw(width);   
            stream << valsub; 
        }
        else
        {
            stream << value;  
        }
        stream.flags(f);
    }
}


void write_x(ostream& stream, Scanner* scanner, size_t repeat)
{
    consume(scanner); // consume X

    // print whitespace
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        stream << " ";
    }
}


void write_str(ostream& stream, Scanner* scanner)
{
    consume(scanner); // consume opening '
    for (;;)
    {
        // TODO: deal with '' escape
        char c = peek(scanner);
        if ('\'' == c)
        {
            break;
        }
        else
        {
            advance(scanner);
        }
    }
    // extract string before last '
    char valstr[MAX_STR_LEN];
    extract(scanner, valstr, MAX_STR_LEN);

    advance(scanner);
    consume(scanner); // consume last '

    // print user string
    stream << valstr;
}


void write_nl(ostream& stream, Scanner* scanner)
{
    consume(scanner); // consume the slash
    stream << '\n';
}


void write_h(ostream& stream, Scanner* scanner, size_t length)
{
    consume(scanner); // consume opening '
    for (size_t count = 0; count < length; ++count)
    {
        // TODO: deal with '' escape. count it as 1 char.
        advance(scanner);
    }
    // extract string before last '
    char valstr[MAX_STR_LEN];
    extract(scanner, valstr, MAX_STR_LEN);

    // consume last '
    advance(scanner);
    consume(scanner); 

    // print user string
    stream << valstr;
}


//
// Number formatting
//

void fill_with_char(char* put, char const fill, size_t const width)
{
    for (size_t n = 0; n < width; ++n)
    {
        put[n] = fill;
    }
}

void format_i(char* put, int value, size_t width, size_t fill)
{
    unsigned int absvalue = abs(value);

    size_t len = integer_str_length(absvalue);

    bool require_sign = value < 0;
    bool fill_chars   = len < fill;

    size_t maxlen = len + require_sign + fill_chars*(fill - len);

    size_t pos = 0;
    if (maxlen > width)
    {
        fill_with_char(put, '*', width);
        pos = pos + width;
    }
    else
    {
        // right-alignment whitespace
        for (;pos < width - maxlen; ++pos)
        {
            put[pos] = ' ';
        }

        // minus sign
        if (require_sign)
        {
            put[pos] = '-';
            pos = pos + 1;
        }

        // leading zeroes
        if (fill_chars)
        {
            for (size_t fillcount = 0; fillcount < fill - len; ++fillcount, ++pos)
            {
                put[pos] = '0';
            }
        }

        // integer
        // TODO: can be refactored, see extract_integer_part
        unsigned int intpart = abs(static_cast<int>(value));
        for(size_t n = 0; n < len; ++n, ++pos)
        {
            size_t power = fast_10pow(len - n - 1);
            unsigned int newvalue = static_cast<int>(intpart / power);
            put[pos] = newvalue + '0';

            intpart = intpart - newvalue * power;
        }
    }
    put[pos] = '\0'; 
}


void format_f(char* put, double value, size_t width, size_t precision)
{
    const char FILL_CHAR = '*';
    const char DOT_CHAR = '.';

    // integer part
    int intval = abs(static_cast<int>(value));
    char intvalstr[MAX_STR_LEN];
    extract_integer_part(intvalstr, intval);
    size_t intlen = strlen(intvalstr);

    // fractional part
    char fractstr[MAX_STR_LEN];
    extract_decimal_part(fractstr, value, precision);

    // whether the sign is present
    bool require_sign = value < 0; 
    // TODO: this test doesn't handle -0.0
    // stream << "Sign" << require_sign << '\n';

    // the integer part output is optional when its = 0
    bool optional_intvalue = 0 == intval;

    // the required string width, given the integral part, the dot and the 
    // precision
    size_t strwidth = require_sign + intlen + 1 + precision;

    if (strwidth > width and strwidth - width == 1 and optional_intvalue and width != 1)
    {
        // omit integral part and then write the number
        size_t pos = 0;
        if (require_sign)
        {
            put[pos] = '-';
            pos = pos + 1;
        }
        put[pos] = DOT_CHAR;
        pos = pos + 1;

        size_t const fraclen = strlen(fractstr);
        size_t const maxchars = width - 1; // max. chars that can be written
        size_t total = maxchars;

        if (fraclen < maxchars)
        {
            total = fraclen;
        }

        for (; pos < total + 1 + require_sign; ++pos)
        {
            put[pos] = fractstr[pos - 1 -require_sign];
        }
        put[total + 1 + require_sign] = '\0';
    }
    else if (strwidth <= width)
    {
        // write the number
        size_t pos = 0;
        if (require_sign)
        {
            put[pos] = '-';
            pos = pos + 1;
        }
        // integral part
        
        for (; pos < intlen + require_sign; ++pos)
        {
            put[pos] = intvalstr[pos - require_sign];
        }
        put[intlen + require_sign] = DOT_CHAR;
        // fractional part
        pos = intlen + 1 + require_sign;
        size_t const fraclen = strlen(fractstr);
        size_t const maxchars = width - intlen - 1 - require_sign; // max. chars that can be written
        size_t total = maxchars;

        if (fraclen < maxchars)
        {
            total = fraclen;
        }

        for (; pos < total + intlen + 1 + require_sign; ++pos)
        {
            put[pos] = fractstr[pos - intlen - 1 - require_sign];
        }
        put[total + intlen + 1 + require_sign] = '\0';
    }
    else
    {
        // fill with FILL_CHAR
        for (size_t count = 0; count < strwidth; ++count)
        {
            put[count] = FILL_CHAR;
        }
        put[width] = '\0';
    }
}


void format_d(char* put, double value, size_t width, size_t precision, char expchar)
{
    double absvalue = value;
    if (value < 0)
    {
        absvalue = -value;
    }
    bool require_sign = value < 0;

    // 0. precision D+00
    size_t const FORMATCHARS = 6;
    size_t maxlen = require_sign + FORMATCHARS + precision;

    size_t pos = 0;
    if (maxlen > width)
    {
        fill_with_char(put, '*', width);
        pos = pos + width;
    }
    else
    {
        // calculate the decimal part and the exponent
        double finalnumber;
        int exponent;
        double power;
        if (absvalue >= 1)
        {
            // numbers with the integer part
            unsigned int intval = static_cast<unsigned int>(absvalue);
            exponent = integer_str_length(intval);

            if (exponent >= precision)
            {
                power = 1.0 / static_cast<double>(fast_10pow(exponent - precision));
            }
            else
            {
                power = static_cast<double>(fast_10pow(precision - exponent));
            }
        }
        else
        {
            // numbers of zeroes in the fractional part
            exponent = frac_zeroes(absvalue);
            power = static_cast<double>(fast_10pow(exponent + precision));

            exponent = - exponent;
        }
        // final number after the decimal separator
        finalnumber = absvalue * power;
        unsigned int intval = static_cast<unsigned int>(finalnumber);

        // right-alignment whitespace
        if (width > maxlen)
        {
            for (;pos < width - maxlen; ++pos)
            {
                put[pos] = ' ';
            } 
        }

        // minus sign
        if (require_sign)
        {
            put[pos] = '-';
            pos = pos + 1;
        }

        // leading zero and dot
        put[pos]     = '0';
        put[pos + 1] = '.';
        pos = pos + 2;

        // number value
        format_i(put + pos, intval, precision, 0);
        pos = pos + precision;

        // exponent sign
        put[pos] = expchar;
        pos = pos + 1;
        if (exponent >= 0.0)
        {
            put[pos] = '+';
        }
        else
        {
            put[pos] = '-';
        }
        pos = pos + 1;

        // exponent value
        format_i(put + pos, abs(exponent), 2, 2);
        pos = pos + 2;
    }
    put[pos] = '\0';
}


size_t fast_10pow(size_t const exponent)
{
    static size_t pow10[10] = {
        1, 10, 100, 1000, 10000, 
        100000, 1000000, 10000000, 100000000, 1000000000
    };

    if (exponent <= 9)
    {
        // fast, look-up
        return pow10[exponent];
    }
    else
    {
        // generic
        size_t powered = 1;
        for (size_t n=0; n < exponent; ++n)
        {
            powered = powered * 10;
        }
        return powered;
    }
}


size_t integer_str_length(unsigned int value)
{
    // general solution (using it for big numbers)
    if (value >= 10000000) return floor(log10(value)) + 1;
    // ugly, but optimal
    // credits: https://stackoverflow.com/a/3069580
    if (value >= 100000 )  return 6;
    if (value >= 10000  )  return 5;
    if (value >= 1000   )  return 4;
    if (value >= 100    )  return 3;
    if (value >= 10     )  return 2;
    return 1;
}


void extract_integer_part(char* put, double value)
{
    // TODO: can be refactored, see format_i
    unsigned int intpart = abs(static_cast<int>(value));
    size_t const digits = integer_str_length(abs(intpart));

    for(size_t pos = 0; pos < digits; ++pos)
    {
        size_t power = fast_10pow(digits - pos - 1);
        unsigned int newvalue = static_cast<int>(intpart / power);
        put[pos] = newvalue + '0';

        intpart = intpart - newvalue * power;
    }
    put[digits] = '\0';
}


size_t frac_zeroes(double value)
{   
    if (value >= 1.0)
    {
        return 0;
    }
    double absvalue = value;
    if (absvalue < 0)
    {
        absvalue = -value;
    }
    // general solution (using it for small numbers)
    if (absvalue < 0.000001) return -floor(log10(absvalue)) + 1;
    // ugly, but optimal
    // similar: https://stackoverflow.com/a/3069580
    if (absvalue < 0.000001)  return 6;
    if (absvalue < 0.00001 )  return 5;
    if (absvalue < 0.0001  )  return 4;
    if (absvalue < 0.001   )  return 3;
    if (absvalue < 0.01    )  return 2;
    if (absvalue < 0.1     )  return 1;
    return 0;
}



void extract_decimal_part(char* put, double value, size_t precision)
{
    double absvalue = value;
    if (absvalue < 0)
    {
        absvalue = -absvalue;
    }
    int intpart = static_cast<int>(absvalue);
    double decpart = absvalue - intpart;

    // power of ten
    double power = 1;
    for (size_t pw = 1; pw <= precision; ++pw)
    {
        power = power * 10;
        decpart = absvalue * power - static_cast<int>(absvalue*(power/10))*10;
        intpart = static_cast<int>(decpart);
        put[pw-1] = intpart + '0';
    }
    put[precision] = '\0';
}


//
// Scanner functions
//

void consume(Scanner* scanner)
{
    scanner->start = scanner->current;
}


void extract(Scanner* scanner, char* put, size_t length)
{
    size_t len = (scanner->current - scanner->start) + 1;
    if (len > length)
    {
        len = length;
    }
    strncpy(put, scanner->start, len);
    put[len-1] = '\0';

    consume(scanner);
}


inline char advance(Scanner* scanner)
{
    scanner->current++;
    return scanner->current[-1];
}


bool match(Scanner* scanner, char expected)
{
    if (isAtEnd(scanner))
    {
        return false;
    }
    if (scanner->current[0] != expected)
    {
        return false;
    }

    scanner->current++;
    return true;
}


inline char peek(Scanner* scanner)
{
    return scanner->current[0];
}


inline char peekNext(Scanner* scanner)
{
    if (isAtEnd(scanner))
    {
        return '\0';
    }
    return scanner->current[1];
}


bool isAtEnd(Scanner* scanner)
{
    switch (scanner->current[0])
    {
        case '\0':  // c null terminator character
        case '/':  // colon edit descriptor
        // case ':':  // slash edit descriptor
            return true;
        default:
            return false;
    }
}


inline bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}


inline bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}


int integer(Scanner* scanner)
{
    int val = 0;

    while(isDigit(peek(scanner)))
    {
        advance(scanner);
    }

    char valstr[MAX_STR_LEN];
    extract(scanner, valstr, MAX_STR_LEN);
    scanner->start = scanner->current;

    val = atoi(valstr);

    return val;
}


void skipWhitespace(Scanner* scanner)
{
    for (;;)
    {
        char c = peek(scanner);
        switch (c)
        {
            case ' ':
            case '\r':
            case '\t':
            case '\n': // is this expected?
                advance(scanner);
                break;
            default:
                return;
        }
    }
}
