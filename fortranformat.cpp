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


void  write_i(Scanner* scanner, va_list ap);
void  write_f(Scanner* scanner, va_list ap);

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
    unsigned int repeat = 0;

    va_list ap;
    va_start(ap, formatstr);
    for (;;)
    {
        char c = advance(&scanner);
        // edit descriptors
        if (isAlpha(c))
        {
            switch(c)
            {
                case 'F':
                    write_f(&scanner, ap); 
                break;

                case 'I':
                    write_i(&scanner, ap); 
                break;
            }
        }
        // repeat number
        if (isDigit(c))
        {
            repeat = integer(&scanner);
        }

        if (isAtEnd(&scanner))
        {
            break;
        }
    }
    va_end(ap);

    std::cout << std::endl;
}


//
// Format write edit descriptors
//

void  write_i(Scanner* scanner, va_list ap)
{
    int value = va_arg(ap, int); 

    consume(scanner);

    int width = integer(scanner);
    int atleast = 0;
    
    if (peek(scanner) == '.')
    {
        advance(scanner);
        consume(scanner);
        atleast = integer(scanner);
    }
    // pop arg value
    std::ios_base::fmtflags f(std::cout.flags());
    std::cout << std::right << std::setw(width) << value;
    std::cout.flags(f);
}


void  write_f(Scanner* scanner, va_list ap)
{
    double value = va_arg(ap, double); 

    consume(scanner);

    int width = integer(scanner);
    int decimal = 0;

    // dot character
    advance(scanner); 
    consume(scanner);
    decimal = integer(scanner);
    
    // pop arg value
    std::ios_base::fmtflags f(std::cout.flags());
    std::cout << std::right;
    std::cout.setf(std::ios::floatfield, std::ios::fixed);
    std::cout << std::setw(width) << std::setprecision(decimal) << value; 
    std::cout.flags(f);
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
