#ifndef __GENERAL_H__
#define __GENERAL_H__

struct streqpred
{
   bool operator()(const char* a, const char* b) const
   {
      return strcmp(a, b) == 0;
   }
};

#endif // __GENERAL_H__