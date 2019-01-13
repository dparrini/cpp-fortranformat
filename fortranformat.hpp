#ifndef H_FORTRANFORMAT_
#define H_FORTRANFORMAT_

union values_t
{
    char * chval;
    long int intval;
    long double dblval;
};


void write(char const* formatstr ...);

#ifdef DEBUG

void tests();

#endif

#endif