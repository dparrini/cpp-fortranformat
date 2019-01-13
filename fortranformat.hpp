#ifndef H_FORTRANFORMAT_
#define H_FORTRANFORMAT_


void write(char const* formatstr, ...);

#ifdef DEBUG
void tests();
#endif

#endif