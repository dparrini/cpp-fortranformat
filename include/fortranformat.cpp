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


#include <cassert>
#include <cmath>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <ostream>
#include "fortranformat.hpp"
using std::ostream;

// default buffer size
size_t const MAX_STR_LEN = 200;

// default exponent width
size_t const DEFAULT_EXPONENT = 2;

// Fortran's output for true/false values
char const FORTRAN_TRUE  = 'T';
char const FORTRAN_FALSE = 'F';

// Floating point numbers exponential part character
char const EXPONENTIAL_D = 'D';
char const EXPONENTIAL_E = 'E';

// character to fill when the number width overflows specified width
char const OVERFLOW_FILL_CHAR = '*';


struct Scanner {
    const char* start;
    const char* current;

    Scanner(const char* const source)
    {
        this->start   = source;
        this->current = source;
    }
};


//
// Scanner functions
//

void consume(Scanner* const scanner)
{
    scanner->start = scanner->current;
}


void extract(Scanner* const scanner, char* put, size_t const length)
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


inline char advance(Scanner* const scanner)
{
    scanner->current++;
    return scanner->current[-1];
}


bool is_at_end(Scanner const* const scanner)
{
    return '\0' == scanner->current[0];
}


bool match(Scanner* const scanner, char const expected)
{
    if (is_at_end(scanner))
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


inline char peek(Scanner const* const scanner)
{
    return scanner->current[0];
}


inline char peek_next(Scanner const* const scanner)
{
    if (is_at_end(scanner))
    {
        return '\0';
    }
    return scanner->current[1];
}


inline bool is_digit(char const c)
{
    return c >= '0' && c <= '9';
}


inline bool is_alpha(char const c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}


int integer(Scanner* const scanner)
{
    int val = 0;

    while(is_digit(peek(scanner)))
    {
        advance(scanner);
    }

    char valstr[MAX_STR_LEN];
    extract(scanner, valstr, MAX_STR_LEN);
    scanner->start = scanner->current;

    val = atoi(valstr);

    return val;
}


void skip_whitespace(Scanner* const scanner)
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


//
// Numbers auxiliary functions
//

inline bool is_negative(double const value)
{
    // a function that handles 0.0 and -0.0 (the test -0.0 < 0.0 fails)
    return std::signbit(value);
}


double fast_10pow(size_t const exponent)
{
    static double pow10[10] = {
        1.0, 10.0, 100.0, 1000.0, 10000.0, 
        100000.0, 1000000.0, 10000000.0, 100000000.0, 1000000000.0
    };

    if (exponent <= 9)
    {
        // fast, look-up
        return pow10[exponent];
    }
    else
    {
        // generic
        double powered = 1.0;
        for (size_t n=0; n < exponent; ++n)
        {
            powered = powered * 10.0;
        }
        return powered;
    }
}


size_t integer_str_length(unsigned int const value)
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


void write_integer(char* put, double const value, 
    bool const rounded_last_digit, bool const insert_nul)
{
    double absvalue = fabs(value);
    unsigned int intpart = abs(static_cast<int>(value));
    size_t const digits = integer_str_length(abs(intpart));

    for(size_t pos = 0; pos < digits; ++pos)
    {
        size_t power = static_cast<size_t>(fast_10pow(digits - pos - 1));
        unsigned int newvalue = static_cast<int>(intpart / power);

        // round last digit
        if (rounded_last_digit && pos == digits - 1)
        {
            double lastval = round(absvalue - 
                static_cast<int>(absvalue/10.0) * 10.0);
            newvalue = static_cast<int>(lastval);
        }

        put[pos] = newvalue + '0';

        intpart = intpart - newvalue * power;
    }  
    if (insert_nul)
    {
        put[digits] = '\0';
    }
}


int base10_exponent(double const value)
{
    double const absvalue = fabs(value);

    // general case, absvalue >= 1E+6 or < 1E-6
    if (absvalue >=  1000000.0 || absvalue < 0.000001)
    {
        return floor(log10(absvalue));
    }
    else
    {
        if (absvalue >= 100000.0)  return  5;  
        if (absvalue >= 10000.0 )  return  4;
        if (absvalue >= 1000.0  )  return  3;
        if (absvalue >= 100.0   )  return  2;
        if (absvalue >= 10.0    )  return  1;
        if (absvalue >=  1.0    )  return  0;
        if (absvalue >=  0.1    )  return -1;  
        if (absvalue >=  0.01   )  return -2;
        if (absvalue >=  0.001  )  return -3;
        if (absvalue >=  0.0001 )  return -4;
        if (absvalue >=  0.00001)  return -5;
        return -6;
    }
}


size_t frac_zeroes(double const value)
{   
    double const absvalue = fabs(value);
    if (absvalue >= 0.1)
    {
        return 0;
    }
    else
    {
        return -base10_exponent(value) -1;
    }
}


void extract_fractional_part(char* put, double const value, 
    size_t const precision, bool const rounded_last_digit)
{
    double absvalue = fabs(value);

    if (rounded_last_digit)
    {
        double last_digit = fast_10pow(precision) * absvalue;
        // last digit under specified precision, multiplied by 10
        int last_digit_int = static_cast<int>(last_digit) * 10;
        double digit_after = fast_10pow(precision + 1) * absvalue - last_digit_int;

        if (static_cast<int>(fabs(round(digit_after))) >= 5)
        {
            absvalue = absvalue + 1.0 / static_cast<double>(fast_10pow(precision));
        }
    }

    int intpart = static_cast<int>(absvalue);
    double decpart = absvalue - intpart;

    // power of ten
    double power = 1;
    for (size_t pw = 0; pw < precision; ++pw)
    {
        power = power * 10;
        decpart = absvalue * power - static_cast<int>(absvalue*(power/10))*10;
        intpart = static_cast<int>(decpart);
        put[pw] = intpart + '0';
    }
    put[precision] = '\0';
}


inline double fabs(double const value)
{
    if (is_negative(value))
    {
        return -value;
    }
    return value;
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


bool format_sign(Scanner* scanner, bool const opt_plus_sign)
{
    bool plus_sign = opt_plus_sign;
    // consume the first S
    consume(scanner);

    if (peek(scanner) == 'P')
    {
        plus_sign = true;
    }
    // TODO: test for whitespace/delimiter (matching only the first S)
    else if (peek(scanner) == 'S')
    {
        plus_sign = false; 
    }

    advance(scanner);
    consume(scanner);

    return plus_sign;
}


void format_i(char* put, int const value, size_t const width, size_t const fill,
    bool const plus_sign)
{
    unsigned int absvalue = abs(value);

    size_t len = integer_str_length(absvalue);

    bool require_sign = is_negative(value) || plus_sign;
    bool fill_chars   = len < fill;

    size_t maxlen = len + require_sign + fill_chars*(fill - len);

    size_t pos = 0;
    if (maxlen > width)
    {
        fill_with_char(put, OVERFLOW_FILL_CHAR, width);
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
            if (is_negative(value))
            {
                put[pos] = '-'; 
            }
            else
            {
                put[pos] = '+';   
            }
            
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
        bool const NOT_ROUNDED = false;
        bool const INSERT_NUL = false;
        write_integer(put + pos, value, NOT_ROUNDED, INSERT_NUL);
    }
    put[width] = '\0'; 
}


void format_f(char* put, double const value, size_t const width, 
    size_t const precision, bool const plus_sign)
{
    bool require_sign = is_negative(value) || plus_sign;

    // integer part
    int intval = abs(static_cast<int>(value));
    char intstr[MAX_STR_LEN];
    bool const NOT_ROUNDED = false;
    bool const INSERT_NUL = true;
    write_integer(intstr, intval, NOT_ROUNDED, INSERT_NUL);
    size_t const INTLEN = strlen(intstr);

    // the integer part output is optional when its = 0
    bool optional_intvalue = 0 == intval;

    // fractional part
    char fracstr[MAX_STR_LEN];
    bool const ROUNDED = true;
    extract_fractional_part(fracstr, value, precision, ROUNDED);
    size_t const FRACLEN = precision;

    // 0. precision D+00
    size_t const FORMATCHARS = 1;
    size_t const INTCHARS = (!optional_intvalue) * INTLEN;
    size_t maxlen = require_sign + INTCHARS + FORMATCHARS + FRACLEN;


    if (width < 2 || maxlen > width)
    {
        fill_with_char(put, OVERFLOW_FILL_CHAR, width);
    }
    else if (precision == 0)
    {
        // an integer with a following dot
        int rounded_intval = static_cast<int>(round(value));
        if (rounded_intval == 0 && is_negative(value))
        {
            // handles the weird case where value = -0.0 and its int cast
            // is positive 0.
            if (width >= 3)
            {              
                size_t pos = 0;
                size_t len = 2 + require_sign + precision;
                // right-alignment whitespace
                if (width > len)
                {
                    for (;pos < width - len; ++pos)
                    {
                        put[pos] = ' ';
                    } 
                }
                // minus or plus sign
                if (require_sign)
                {
                    if (is_negative(value))
                    {
                        put[pos] = '-'; 
                    }
                    else
                    {
                        put[pos] = '+';
                    }
                    pos = pos + 1;
                }
                put[pos] = '0';
                put[width - 1] = '.'; 
            }
            else
            {
                fill_with_char(put, OVERFLOW_FILL_CHAR, width);
            }
        }
        else
        {
            format_i(put, rounded_intval, width - 1, 0, plus_sign);
            put[width - 1] = '.';
        }
    }
    else
    {
        // actual length, possibly including a leading zero for x < 1.0
        size_t len = maxlen;
        bool include_leading_zero = width > len && optional_intvalue;
        if (include_leading_zero)
        {
            // include optional 0 in the maxlen, if its not accounted for initially
            len = len + 1;
        }
        size_t pos = 0;

        // right-alignment whitespace
        if (width > len)
        {
            for (;pos < width - len; ++pos)
            {
                put[pos] = ' ';
            } 
        }

        // minus sign
        if (require_sign)
        {
            if (is_negative(value))
            {
                put[pos] = '-'; 
            }
            else
            {
                put[pos] = '+';
            }
            
            pos = pos + 1;
        } 

        // leading zero
        if (include_leading_zero)
        {
            put[pos] = '0';
            pos = pos + 1;
        }
        else if (intval != 0)
        {
            // integer part value
            for(size_t n = 0; n < INTLEN; ++n, ++pos)
            {
                put[pos] = intstr[n];
            }
        }
        put[pos] = '.';
        pos = pos + 1;

        // fractional part value
        for(size_t n = 0; n < FRACLEN; ++n, ++pos)
        {
            put[pos] = fracstr[n];
        }   
    }
    put[width] = '\0';
}


void format_e(char* put, double const value, size_t const width, 
    size_t const precision, char const expchar, size_t const exponent_width,
    bool const plus_sign)
{
    assert(exponent_width > 0);

    double absvalue = fabs(value);
    bool require_sign = is_negative(value) || plus_sign;

    // minimum length without leading zero
    // . precision D+00, ., D, and + = 3
    size_t const FORMATCHARS = 3;
    size_t const MINLEN = require_sign + FORMATCHARS + precision + exponent_width;

    // check whether there's space for a leading zero
    bool include_leading_zero = width > MINLEN;
    size_t const LEN = MINLEN + include_leading_zero;

    size_t pos = 0;
    if (LEN > width)
    {
        fill_with_char(put, OVERFLOW_FILL_CHAR, width);
        pos = pos + width;
    }
    else
    {
        // calculate the fractional part and the exponent
        double finalnumber;
        int const exponent = base10_exponent(value);
        // print exponent, since the number starts with 0.
        int const expfort = exponent + 1;
        // power of 10 to extract the values to print in the space given the precision
        double power;
        if (exponent >= 0)
        {
            if (expfort >= static_cast<int>(precision))
            {
                power = 1.0 / static_cast<double>(fast_10pow(expfort - precision));
            }
            else
            {
                power = static_cast<double>(fast_10pow(precision - expfort));
            }
        }
        else
        {
            // -exponent + 1 = numbers of zeroes in the fractional part
            int const zeroes = -(exponent + 1);
            power = static_cast<double>(fast_10pow(zeroes + precision));
        }
        // final number after the decimal separator

        finalnumber = absvalue * power;
        unsigned int intval = static_cast<unsigned int>(round(finalnumber));

        // right-alignment whitespace
        if (width > LEN)
        {
            for (;pos < width - LEN; ++pos)
            {
                put[pos] = ' ';
            } 
        }

        // minus sign
        if (require_sign)
        {
            if (is_negative(value))
            {
                put[pos] = '-'; 
            }
            else
            {
                put[pos] = '+';
            }
            
            pos = pos + 1;
        }

        // leading zero
        if (include_leading_zero)
        {
            put[pos] = '0';
            pos = pos + 1;
        }
        put[pos] = '.';
        pos = pos + 1;

        // number value
        format_i(put + pos, intval, precision, 0, false);
        pos = pos + precision;

        // exponent sign
        put[pos] = expchar;
        pos = pos + 1;
        if (expfort >= 0)
        {
            put[pos] = '+';
        }
        else
        {
            put[pos] = '-';
        }
        pos = pos + 1;

        // exponent value
        format_i(put + pos, abs(expfort), exponent_width, exponent_width, false);
        pos = pos + exponent_width;
    }
    put[pos] = '\0';
}


void format_g(char* put, double const value, size_t const width, 
    size_t const precision, size_t const exponent, bool const plus_sign)
{
    assert(exponent > 0);

    double const MIN = 0.1;
    double const MAX = fast_10pow(precision);
    double absvalue = fabs(value);

    if (MIN > absvalue || absvalue >= MAX)
    {
        // format as Ew.dEe
        format_e(put, value, width, precision, EXPONENTIAL_E, 
            exponent, plus_sign);
    }
    else
    {
        if (width > 2 + exponent)
        {
            // first, check range
            size_t d;
            for (d = 0; d <= precision; ++d)
            {
                double min;
                if (d == 0) min = 0.1;
                else        min = fast_10pow(d-1);
                double max = fast_10pow(d);

                if (min <= absvalue && absvalue < max)
                {
                    break;
                }
            }

            // format as Fw.d (no exponential part)
            size_t w = width - (2 + exponent);
            format_f(put, value, w, precision - d, plus_sign);  

            // complete remaining blank spaces
            for (size_t white = w; white < width; ++white)
            {
                put[white] = ' ';
            }
            put[width] = '\0';
        }
        else
        {
            fill_with_char(put, OVERFLOW_FILL_CHAR, width);
            put[width] = '\0';
        }
    }
}


//
// Format write edit descriptors
//

void write_i(ostream& stream, Scanner* scanner, va_list* ap, 
    size_t const repeat, bool const plus_sign)
{
    consume(scanner);

    int width = integer(scanner);
    assert(width > 0);
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

        format_i(put, value, width, fill, plus_sign);
        stream << put;
    }
}


void write_f(ostream& stream, Scanner* scanner, va_list* ap, 
    size_t const repeat, bool const plus_sign)
{
    consume(scanner);

    int width = integer(scanner);
    assert(width > 0);
    int fractional = 0;

    // dot character
    advance(scanner); 
    consume(scanner);
    fractional = integer(scanner);
    
    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        double value = va_arg(*ap, double); 
        char put[MAX_STR_LEN];

        format_f(put, value, width, fractional, plus_sign);
        stream << put;
    }
}


void write_d(ostream& stream, Scanner* scanner, va_list* ap, 
    size_t const repeat, bool const plus_sign)
{
    // TODO: similar to write_e
    consume(scanner);

    int width = integer(scanner);
    assert(width > 0);
    int fractional = 0;

    // dot character
    advance(scanner); 
    consume(scanner);
    fractional = integer(scanner);
    
    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        double value = va_arg(*ap, double); 
        char put[MAX_STR_LEN];

        format_e(put, value, width, fractional, EXPONENTIAL_D, DEFAULT_EXPONENT, 
            plus_sign);  
        stream << put;
    }
}


void write_e(ostream& stream, Scanner* scanner, va_list* ap, 
    size_t const repeat, bool const plus_sign)
{
    // TODO: similar to write_d
    consume(scanner);

    int width = integer(scanner);
    assert(width > 0);
    int fractional = 0;

    // dot character
    advance(scanner); 
    consume(scanner);
    fractional = integer(scanner);

    size_t exponent = DEFAULT_EXPONENT;
    if (peek(scanner) == 'E')
    {
        advance(scanner); 
        consume(scanner);
        exponent = integer(scanner);
    }
    
    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        double value = va_arg(*ap, double); 
        char put[MAX_STR_LEN];

        format_e(put, value, width, fractional, EXPONENTIAL_E, exponent, 
            plus_sign);  
        stream << put;
    }
}


void write_g(ostream& stream, Scanner* scanner, va_list* ap, 
    size_t const repeat, bool const plus_sign)
{
    // TODO: similar to write_d and write_e
    consume(scanner);

    int width = integer(scanner);
    assert(width > 0);
    int fractional = 0;

    // dot character
    advance(scanner); 
    consume(scanner);
    fractional = integer(scanner);

    size_t exponent = DEFAULT_EXPONENT;
    if (peek(scanner) == EXPONENTIAL_E)
    {
        advance(scanner); 
        consume(scanner);
        exponent = integer(scanner);
    }
    
    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        double value = va_arg(*ap, double); 
        char put[MAX_STR_LEN];

        format_g(put, value, width, fractional, exponent, plus_sign);  
        stream << put;
    }
}


void write_l(ostream& stream, Scanner* scanner, va_list* ap, 
    size_t const repeat)
{
    consume(scanner);
    size_t width = integer(scanner);
    assert(width > 0);

    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        int value = va_arg(*ap, int);

        size_t pos = 0;
        char valstr[MAX_STR_LEN];
        for (;pos < width - 1; ++pos)
        {
            valstr[pos] = ' ';
        }
        if (value)
        {
            valstr[pos] = FORTRAN_TRUE;
        }
        else
        {
            valstr[pos] = FORTRAN_FALSE;   
        }
        valstr[width] = '\0';
        stream << valstr;
    }
}


void write_a(ostream& stream, Scanner* scanner, va_list* ap, 
    size_t const repeat)
{
    consume(scanner);
    size_t width = 0;
    if (is_digit(peek(scanner)))
    {
        // if the user specify a width, it must be nonzero
        width = integer(scanner);
        assert(width > 0);
    }

    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        char const* value = va_arg(*ap, char const*); 
        size_t value_width = strlen(value);

        if (width > 0)
        {
            char valsub[MAX_STR_LEN];

            size_t pos = 0;
            size_t whats_left = width;
            if (width > value_width)
            {
                // pad with whitespace
                for (; pos < width - value_width; ++pos)
                {
                    valsub[pos] = ' ';
                }
                whats_left = value_width;
            }
            
            strncpy(valsub + pos, value, whats_left);
            valsub[width] = '\0';

            stream << valsub; 
        }
        else
        {
            stream << value;  
        }
    }
}


void write_x(ostream& stream, Scanner* scanner, 
    size_t const repeat)
{
    consume(scanner); // consume X

    // print whitespace
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        stream << " ";
    }
}


void write_str(ostream& stream, Scanner* scanner, char const opening)
{
    // extracted string
    char valstr[MAX_STR_LEN];
    // pointer to current character being written
    char* strinit = valstr;

    size_t length_left = MAX_STR_LEN;

    consume(scanner); // consume opening ' or "
    for (;;)
    {
        // TODO: test length_left for 0 value or underflow
        char c = peek(scanner);
        if (opening == c)
        {
            if (peek_next(scanner) != opening)
            {
                break;
            }
            else
            {
                // consume one of the quotation marks
                advance(scanner);
                // copy characters until the first quotation mark
                extract(scanner, strinit, length_left);
                // move pointer
                strinit = strinit + strlen(strinit);
                length_left = MAX_STR_LEN - (strinit - valstr);
                // jump escape character
                advance(scanner);
                consume(scanner);
            }
        }
        else
        {
            advance(scanner);
        }
    }
    // extract string before last '
    
    extract(scanner, strinit, length_left);

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


void write_h(ostream& stream, Scanner* scanner, 
    size_t const length)
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


bool write_group(ostream& stream, Scanner* scanner, va_list* ap, 
    bool const plus_sign)
{
    // consume open paren and following whitespace
    skip_whitespace(scanner);
    consume(scanner);

    // force optional plus sign for I, F, D, E, G descriptors
    bool opt_plus_sign = plus_sign;

    for (;;)
    {
        char c = advance(scanner);

        // repeat count
        unsigned int repeat = 1;
        if (is_digit(c))
        {
            repeat = integer(scanner);
            assert(repeat > 0);
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
                opt_plus_sign = write_group(stream, scanner, ap, opt_plus_sign);    
            }
        }
        // edit descriptors
        else if (is_alpha(c))
        {
            switch(c)
            {
                case 'A':
                    write_a(stream, scanner, ap, repeat); 
                break;

                case 'D':
                    write_d(stream, scanner, ap, repeat, opt_plus_sign); 
                break;

                case 'E':
                    write_e(stream, scanner, ap, repeat, opt_plus_sign); 
                break;

                case 'F':
                    write_f(stream, scanner, ap, repeat, opt_plus_sign); 
                break;

                case 'G':
                    write_g(stream, scanner, ap, repeat, opt_plus_sign); 
                break;

                case 'H':
                    write_h(stream, scanner, repeat); 
                break;

                case 'I':
                    write_i(stream, scanner, ap, repeat, opt_plus_sign); 
                break;

                case 'L':
                    write_l(stream, scanner, ap, repeat); 
                break;

                case 'S':
                    opt_plus_sign = format_sign(scanner, opt_plus_sign); 
                break;

                case 'X':
                    write_x(stream, scanner, repeat); 
                break;
            }
        }
        else if ('/' == c)
        {
            write_nl(stream, scanner);
        }
        else if ('\'' == c)
        {
            write_str(stream, scanner, '\'');
        }
        else if ('"' == c)
        {
            write_str(stream, scanner, '"');
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

        if (is_at_end(scanner))
        {
            break;
        }

        skip_whitespace(scanner);
        consume(scanner);
    }

    // spill group options
    return opt_plus_sign;
}


//
// Public Interface
//

void stream_printfor(ostream& stream, char const* const formatstr, va_list* ap)
{
    bool const OPTIONAL_PLUS_SIGN = false;

    Scanner scanner(formatstr);
    skip_whitespace(&scanner);
    char c = advance(&scanner);
    if ('(' == c)
    {
        write_group(stream, &scanner, ap, OPTIONAL_PLUS_SIGN);
    }
    stream << '\n';
}


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

