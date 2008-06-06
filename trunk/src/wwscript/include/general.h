#ifndef __GENERAL_H__
#define __GENERAL_H__

#include <string.h>
#include <string>

struct streqpred
{
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

_STLP_BEGIN_NAMESPACE

_STLP_TEMPLATE_NULL struct hash<std::string>
{
	size_t operator()(std::string __s) const {
		unsigned long __h = 0; 
		for (unsigned int i = 0; i < __s.length(); ++i)
			__h = 5*__h + __s[i];

		return size_t(__h);
	}
};

_STLP_END_NAMESPACE

#endif // __GENERAL_H__