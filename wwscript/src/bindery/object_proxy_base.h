#if !BOOST_PP_IS_ITERATING
  #ifndef OBJECT_PROXY_BASE_H
  #define OBJECT_PROXY_BASE_H
  
  #include "calling_convention.h"
  #include <boost/preprocessor/repetition/enum_binary_params.hpp>

  template <typename T>
  class ObjectProxyBase {
    protected:
      ObjectProxyBase() : ref_cnt(1) {}
      ObjectProxyBase(const ObjectProxyBase & other) : object(other.object), ref_cnt(1) {}
      ObjectProxyBase & operator=(const ObjectProxyBase & rhs) {
        object = rhs.object;
        return *this;
      }

      #define BOOST_PP_ITERATION_PARAMS_1 (3, (0, MAX_ARGS, "object_proxy_base.h"))
      #include BOOST_PP_ITERATE()

      T object;
      uint32 ref_cnt;
      
  };
  #endif
#elif BOOST_PP_ITERATION_DEPTH() == 1
  #define NUM_ARGS BOOST_PP_FRAME_ITERATION(1)
  #if NUM_ARGS != 0
    template <BOOST_PP_ENUM_PARAMS(NUM_ARGS, typename A)>
    ObjectProxyBase(BOOST_PP_ENUM_BINARY_PARAMS(NUM_ARGS, A, a))
      : object(BOOST_PP_ENUM_PARAMS(NUM_ARGS, a)), ref_cnt(1) {}
  #endif
#endif
