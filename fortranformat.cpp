#include <cmath>
#include <cstdarg>
#include <cstring>
#include <iomanip>
#include <iostream>
#include "fortranformat.hpp"


size_t const MAXLEN = 200;


struct Scanner {
    const char* start;
    const char* current;

    Scanner(const char* source)
    {
        this->start   = source;
        this->current = source;
    }
};


void write_group(Scanner* scanner, va_list* ap);
void write_i(Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_f(Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_d(Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_e(Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_g(Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_l(Scanner* scanner, va_list* ap, size_t repeat = 1);
void write_a(Scanner* scanner, va_list* ap, size_t repeat = 1);

void write_x(Scanner* scanner, size_t repeat = 1);
void write_str(Scanner* scanner);
void write_h(Scanner* scanner, size_t length);

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


void tests()
{
    char const* fmt = "503 teste";
    Scanner scanner(fmt);
    int num = integer(&scanner);
    std::cout << "Tests" << std::endl;
    std::cout << num << ";" << std::endl;

    std::cout << "Tests (formats)" << std::endl;
    char put[MAXLEN];
    extract_decimal_part(put, 3.14, 2);
    std::cout << put << std::endl;
    extract_decimal_part(put, 3.00, 2);
    std::cout << put << std::endl;
    extract_decimal_part(put, 4.00002, 5);
    std::cout << put << std::endl;

    // lengths
    std::cout << "Tests (lengths)" << std::endl;
    std::cout << integer_str_length(0) << std::endl;
    std::cout << integer_str_length(11) << std::endl;
    std::cout << integer_str_length(100) << std::endl;
    std::cout << integer_str_length(1063) << std::endl;
    std::cout << integer_str_length(100000003) << std::endl;

    // integer parts
    std::cout << "Tests (integer parts)" << std::endl;
    extract_integer_part(put, 0.45);
    std::cout << put << std::endl;
    extract_integer_part(put, 11.3);
    std::cout << put << std::endl;
    extract_integer_part(put, 100.7003);
    std::cout << put << std::endl;
    extract_integer_part(put, 1063.0);
    std::cout << put << std::endl;
    extract_integer_part(put, 100000003.0004);
    std::cout << put << std::endl;

    // write
    size_t width = 5;
    std::cout << std::endl;
    std::cout << "Tests (write)" << std::endl;
    format_f(put, 0.0, 1, 0); // 0.0 -> ___0.
    std::cout << put << std::endl;
    format_f(put, 0.45, width, 3); // 0.450 -> _0.450
    std::cout << put << std::endl;
    format_f(put, 11.3, width, 3); // 11.300 -> *****
    std::cout << put << std::endl;
    format_f(put, 100.7003, width, 1); // 100.700 -> 100.7
    std::cout << put << std::endl;
    format_f(put, 0.0235, width, 4); // 0.0235 -> .0235
    std::cout << put << std::endl;
    format_f(put, 100000003.0004, 10, 0); // 100000003.
    std::cout << put << std::endl;

    // write
    std::cout << std::endl;
    std::cout << "Tests (negative write)" << std::endl;
    format_f(put, -0.0, 1, 0); // -0.0 -> *
    std::cout << put << std::endl;
    format_f(put, -0.0, 2, 0); // -0.0 -> **
    std::cout << put << std::endl;
    format_f(put, -0.0, 3, 0); // -0.0 -> -0.
    std::cout << put << std::endl;
    format_f(put, -0.45, width-1, 3); // -0.450 -> *****
    std::cout << put << std::endl;
    format_f(put, -0.45, width, 3); // -0.450 -> -.450
    std::cout << put << std::endl;
    format_f(put, -0.45, width+1, 3); // -0.450 -> -0.450
    std::cout << put << std::endl;
    format_f(put, -11.3, width+1, 3); // -11.300 -> ******
    std::cout << put << std::endl;
    format_f(put, -100.7003, width+1, 1); // -100.700 -> -100.7
    std::cout << put << std::endl;
    format_f(put, -0.0235, width+1, 4); // -0.0235 -> -.0235
    std::cout << put << std::endl;
    format_f(put, -100000003.0004, 11, 0); // -100000003.
    std::cout << put << std::endl;
}


void printfor(char const* formatstr, ...)
{
    Scanner scanner(formatstr);
    
    va_list ap;
    va_start(ap, formatstr);

    skipWhitespace(&scanner);
    char c = advance(&scanner);
    if ('(' == c)
    {
        write_group(&scanner, &ap);
    }
    va_end(ap);

    std::cout << std::endl;
}


void write_group(Scanner* scanner, va_list* ap)
{
    // consume open paren and following whitespace
    skipWhitespace(scanner);
    consume(scanner);

    for (;;)
    {
        char c = advance(scanner);

        // std::cout << "scanning " << c << std::endl;

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
                write_group(scanner, ap);    
            }
        }
        // edit descriptors
        else if (isAlpha(c))
        {
            switch(c)
            {
                case 'I':
                    write_i(scanner, ap, repeat); 
                break;

                case 'F':
                    write_f(scanner, ap, repeat); 
                break;

                case 'D':
                    write_d(scanner, ap, repeat); 
                break;

                case 'E':
                    write_e(scanner, ap, repeat); 
                break;

                case 'G':
                    write_g(scanner, ap, repeat); 
                break;

                case 'L':
                    write_l(scanner, ap, repeat); 
                break;

                case 'A':
                    write_a(scanner, ap, repeat); 
                break;

                case 'X':
                    write_x(scanner, repeat); 
                break;

                case 'H':
                    write_h(scanner, repeat); 
                break;
            }
        }
        else if ('\'' == c)
        {
            write_str(scanner);
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

void write_i(Scanner* scanner, va_list* ap, size_t repeat)
{
    consume(scanner);

    int width = integer(scanner);
    int atleast = 0;
    
    if (peek(scanner) == '.')
    {
        advance(scanner);
        consume(scanner);
        atleast = integer(scanner);
    }

    // pop arg value(s)
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        int value = va_arg(*ap, int); 
        std::ios_base::fmtflags f(std::cout.flags());
        std::cout << std::right << std::setw(width) << value;
        std::cout.flags(f);  
    }
}


void write_f(Scanner* scanner, va_list* ap, size_t repeat)
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
        char put[MAXLEN];

        format_f(put, value, width, decimal);  
        std::ios_base::fmtflags f(std::cout.flags());
        std::cout << std::setw(width) << std::right << put;
        std::cout.flags(f);  
    }
}


void write_d(Scanner* scanner, va_list* ap, size_t repeat)
{
    // partial support
    write_f(scanner, ap, repeat);
}


void write_e(Scanner* scanner, va_list* ap, size_t repeat)
{
    // partial support
    write_f(scanner, ap, repeat);
}


void write_g(Scanner* scanner, va_list* ap, size_t repeat)
{
    // partial support
    write_f(scanner, ap, repeat);
}


void write_l(Scanner* scanner, va_list* ap, size_t repeat)
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
        std::ios_base::fmtflags f(std::cout.flags());
        std::cout << std::right << std::setw(width) << valstr;
        std::cout.flags(f);
    }
}


void write_a(Scanner* scanner, va_list* ap, size_t repeat)
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
        size_t len = strlen(value);

        std::ios_base::fmtflags f(std::cout.flags());
        if (width > 0)
        {
            char valsub[MAXLEN];
            strncpy(valsub, value, width);
            valsub[width] = '\0';

            std::cout << std::setw(width);   
            std::cout << valsub; 
        }
        else
        {
            std::cout << value;  
        }
        std::cout.flags(f);
    }
}


void write_x(Scanner* scanner, size_t repeat)
{
    consume(scanner); // consume X

    // print whitespace
    for (size_t repcount = 0; repcount < repeat; ++repcount)
    {
        std::cout << " ";
    }
}


void write_str(Scanner* scanner)
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
    char valstr[MAXLEN];
    extract(scanner, valstr, MAXLEN);

    advance(scanner);
    consume(scanner); // consume last '

    // print user string
    std::cout << valstr;
}


void write_h(Scanner* scanner, size_t length)
{
    consume(scanner); // consume opening '
    for (size_t count = 0; count < length; ++count)
    {
        // TODO: deal with '' escape. count it as 1 char.
        advance(scanner);
    }
    // extract string before last '
    char valstr[MAXLEN];
    extract(scanner, valstr, MAXLEN);

    // consume last '
    advance(scanner);
    consume(scanner); 

    // print user string
    std::cout << valstr;
}


//
// Number formating
//

void format_f(char* put, double value, size_t width, size_t precision)
{
    const char FILL_CHAR = '*';
    const char DOT_CHAR = '.';

    // integer part
    int intval = abs(static_cast<int>(value));
    char intvalstr[MAXLEN];
    extract_integer_part(intvalstr, intval);
    size_t intlen = strlen(intvalstr);

    // fractional part
    char fractstr[MAXLEN];
    extract_decimal_part(fractstr, value, precision);

    // whether the sign is present
    bool require_sign = value < 0; 
    // TODO: this test doesn't handle -0.0
    // std::cout << "Sign" << require_sign << std::endl;

    // the integer part output is optional when its = 0
    bool optional_intvalue = 0 == intval;

    size_t minimum_difference = 1 + require_sign;

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
    unsigned int intpart = abs(static_cast<int>(value));
    size_t const digits = integer_str_length(abs(intpart));
    size_t const tenmult = fast_10pow(digits);

    for(size_t pos = 0; pos < digits; ++pos)
    {
        size_t power = fast_10pow(digits - pos - 1);
        unsigned int newvalue = static_cast<int>(intpart / power);
        put[pos] = newvalue + '0';

        intpart = intpart - newvalue * power;
    }
    put[digits] = '\0';
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

    char valstr[MAXLEN];
    extract(scanner, valstr, MAXLEN);
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
