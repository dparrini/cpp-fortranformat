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
}


void write(char const* formatstr, ...)
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
        std::ios_base::fmtflags f(std::cout.flags());
        std::cout << std::right;
        std::cout.setf(std::ios::floatfield, std::ios::fixed);
        std::cout << std::setw(width) << std::setprecision(decimal) << value; 
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
