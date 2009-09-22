#ifndef GLOBAL_FUNCTION_H
#define GLOBAL_FUNCTION_H

#include "calling_convention.h"
#include "traits.h"
#include <boost/utility/enable_if.hpp>

void AS_ThisCallFunctionsCannotBeGlobalFunctions(Undefined &);

template <typename Fn, typename D = void>
struct GlobalFunction {
  GlobalFunction() {
    CallingConvention<Fn>();
  }
};

template <typename Fn>
struct GlobalFunction<Fn, 
                      typename boost::enable_if<
                        boost::integral_constant<bool, 
                          (CallingConvention<Fn>::value == asCALL_CDECL) ||
                          (CallingConvention<Fn>::value == asCALL_STDCALL)
                        >
                      >::type>
  : boost::integral_constant<bool, CallingConvention<Fn>::valid_global_function> 
{
  static std::string signature(const std::string & name) {
    return CallingConvention<Fn>::global_function_signature(name);
  }
};

template <typename Fn>
struct GlobalFunction<Fn, 
                      typename boost::enable_if<
                        boost::integral_constant<bool, 
                          CallingConvention<Fn>::value == asCALL_THISCALL
                        >
                      >::type> {
  GlobalFunction() {
    AS_ThisCallFunctionsCannotBeGlobalFunctions(Fn());
  }
};


void AS_FunctionHasInvalidParameters(boost::true_type) {}

template <typename Fn>
std::string gfs_worker(const std::string & name, Fn, GlobalFunction<Fn> gf) {
  AS_FunctionHasInvalidParameters(gf);
  return GlobalFunction<Fn>::signature(name);
};

template <typename Fn>
std::string global_function_signature(const std::string & name, Fn fn) {
  return gfs_worker(name, fn, GlobalFunction<Fn>());
};

#define REGISTER_GLOBAL_FUNCTION(name, fn) \
  RegisterGlobalFunction( global_function_signature(name, fn).c_str(), \
                          asFUNCTION(fn),                              \
                          calling_convention(fn) )

#endif