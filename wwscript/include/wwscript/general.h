#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <string.h>
#include <string>

struct streqpred
{
   size_t operator()(const char *a) const
   {
	unsigned long h = 0; 
	for ( ; *a; ++a)
	h = 5*h + *a;
     
	return size_t(h);
   }

   bool operator()(const char* a, const char* b) const
   {
      return strcmp(a, b) == 0;
   }
};

struct stdstreqpred
{
	bool operator()(std::string a, std::string b) const
   {
      return a == b;
   }
};

#endif // __GENERAL_H__
