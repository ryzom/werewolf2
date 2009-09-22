#if !BOOST_PP_IS_ITERATING
  #ifndef CALLING_CONVENTION_H
  #define CALLING_CONVENTION_H

  #include "traits.h"
  #include "angelscript.h"
  #include <sstream>
  #include <boost/type_traits.hpp>

  #include <boost/preprocessor/cat.hpp>
  #include <boost/preprocessor/arithmetic/sub.hpp>
  #include <boost/preprocessor/comparison/not_equal.hpp>
  #include <boost/preprocessor/control/if.hpp>  
  #include <boost/preprocessor/iteration/iterate.hpp>
  #include <boost/preprocessor/repetition/enum_params.hpp>
  #include <boost/preprocessor/repetition/enum_trailing_params.hpp>
  #include <boost/preprocessor/repetition/repeat.hpp>

  class Undefined;
  void AS_TypeDoesntSeemToBeAFunction(Undefined &);
  void AS_AngelScriptDoesntSupportFastCall(Undefined &);

  template <typename T>
  struct CallingConvention {
    CallingConvention() {
      AS_TypeDoesntSeemToBeAFunction(T());
    }
  };
  
  template <typename T>
  struct Type2Type {};
  template <typename T>
  void add_const(std::ostream &, T) {}
  template <typename T>
  void add_const(std::ostream & sstr, Type2Type<const T *>) {
    sstr << " const";
  }
  template <typename T>
  void add_const(std::ostream & sstr, Type2Type<const T &>) {
    sstr << " const";
  }

  #define MAX_ARGS 5
  #define BOOST_PP_ITERATION_PARAMS_1 (3, (0, MAX_ARGS, "calling_convention.h"))
  #include BOOST_PP_ITERATE()

  template <typename Fn>
  asDWORD calling_convention(Fn) {
    return CallingConvention<Fn>();
  }

  #endif
#elif BOOST_PP_ITERATION_DEPTH() == 1
  #define EMPTY
  #define NUM_ARGS BOOST_PP_FRAME_ITERATION(1)

  #define AND_ARG(z, n, param) && ASArg<A##n>::value
  #define AND_ARG_NOT_FIRST(z, n, param) \
    BOOST_PP_IF(BOOST_PP_NOT_EQUAL(0, n), AND_ARG(z, n, param), EMPTY)
  #define AND_ARG_NOT_LAST(z, n, param) \
    BOOST_PP_IF(BOOST_PP_NOT_EQUAL(BOOST_PP_SUB(NUM_ARGS, 1), n), AND_ARG(z, n, param), EMPTY)

  #define INSERT_COMMA << ", "
  #define INSERT_COMMA_IF(cond) BOOST_PP_IF(cond, INSERT_COMMA, EMPTY)
  #define INSERT_ARG(z, n, param) \
    INSERT_COMMA_IF(BOOST_PP_NOT_EQUAL(0, n)) << ASArg<A##n>::arg_type()
  #define INSERT_ARG1(z, n, param) \
    INSERT_COMMA_IF(BOOST_PP_NOT_EQUAL(1, n)) << ASArg<A##n>::arg_type()
  #define INSERT_ARG_NOT_FIRST(z, n, param) \
    BOOST_PP_IF(BOOST_PP_NOT_EQUAL(0, n), INSERT_ARG1(z, n, param), EMPTY)
  #define INSERT_ARG_NOT_LAST(z, n, param) \
    BOOST_PP_IF(BOOST_PP_NOT_EQUAL(BOOST_PP_SUB(NUM_ARGS, 1), n), INSERT_ARG(z, n, param), EMPTY)

  #define FIRST_TYPE BOOST_PP_IF(BOOST_PP_NOT_EQUAL(0, NUM_ARGS), A0, void)
  #define LAST_TYPE  BOOST_PP_IF(BOOST_PP_NOT_EQUAL(0, NUM_ARGS), BOOST_PP_CAT(A, BOOST_PP_SUB(NUM_ARGS, 1)), void)

  template <typename R BOOST_PP_ENUM_TRAILING_PARAMS(NUM_ARGS, typename A)>
  struct CallingConvention<R (__cdecl *)(BOOST_PP_ENUM_PARAMS(NUM_ARGS, A))>
    : boost::integral_constant<asDWORD, asCALL_CDECL> {
    
    typedef FIRST_TYPE FirstType;
    static const bool is_valid_firstcall_method
      = ASReturn<R>::value
        BOOST_PP_REPEAT(NUM_ARGS, AND_ARG_NOT_FIRST, ~)
      ;
    static std::string firstcall_signature(const std::string & name) {
      std::stringstream sstr;
      sstr << ASReturn<R>::return_type()
           << " " << name << "("
             BOOST_PP_REPEAT(NUM_ARGS, INSERT_ARG_NOT_FIRST, ~)
           << ")";
      add_const(sstr, Type2Type<FirstType>());
      return sstr.str();
    }
    
    typedef LAST_TYPE LastType;
    static const bool is_valid_lastcall_method
      = ASReturn<R>::value
        BOOST_PP_REPEAT(NUM_ARGS, AND_ARG_NOT_LAST, ~)
      ;
    static std::string lastcall_signature(const std::string & name) {
      std::stringstream sstr;
      sstr << ASReturn<R>::return_type()
           << " " << name << "("
             BOOST_PP_REPEAT(NUM_ARGS, INSERT_ARG_NOT_LAST, ~)
           << ")";
      add_const(sstr, Type2Type<LastType>());
      return sstr.str();
    }
    
    static const bool valid_global_function
      = ASReturn<R>::value
        BOOST_PP_REPEAT(NUM_ARGS, AND_ARG, ~)
      ;
    static std::string global_function_signature(const std::string & name) {
      std::stringstream sstr;
      sstr << ASReturn<R>::return_type()
           << " " << name << "("
             BOOST_PP_REPEAT(NUM_ARGS, INSERT_ARG, ~)
           << ")";
      return sstr.str();
    }
  };
  template <typename R BOOST_PP_ENUM_TRAILING_PARAMS(NUM_ARGS, typename A)>
  struct CallingConvention<R (__stdcall *)(BOOST_PP_ENUM_PARAMS(NUM_ARGS, A))>
    : boost::integral_constant<asDWORD, asCALL_STDCALL> {
    
    static const bool valid_global_function
      = ASReturn<R>::value
        BOOST_PP_REPEAT(NUM_ARGS, AND_ARG, ~)
      ;
    static std::string global_function_signature(const std::string & name) {
      std::stringstream sstr;
      sstr << ASReturn<R>::return_type()
           << " " << name << "("
             BOOST_PP_REPEAT(NUM_ARGS, INSERT_ARG, ~)
           << ")";
      return sstr.str();
    }
  };
  template <typename T, typename R BOOST_PP_ENUM_TRAILING_PARAMS(NUM_ARGS, typename A)>
  struct CallingConvention<R (T::*)(BOOST_PP_ENUM_PARAMS(NUM_ARGS, A))>
    : boost::integral_constant<asDWORD, asCALL_THISCALL> {
    
    typedef T ObjectType;

    static const bool valid_object_method
      = ASReturn<R>::value
        BOOST_PP_REPEAT(NUM_ARGS, AND_ARG, ~)
      ;
    static std::string object_method_signature(const std::string & name) {
      std::stringstream sstr;
      sstr << ASReturn<R>::return_type()
           << " " << name << "("
             BOOST_PP_REPEAT(NUM_ARGS, INSERT_ARG, ~)
           << ")";
      return sstr.str();
    }
  };
  template <typename T, typename R BOOST_PP_ENUM_TRAILING_PARAMS(NUM_ARGS, typename A)>
  struct CallingConvention<R (T::*)(BOOST_PP_ENUM_PARAMS(NUM_ARGS, A)) const>
    : boost::integral_constant<asDWORD, asCALL_THISCALL> {
    
    typedef T ObjectType;

    static const bool valid_object_method
      = ASReturn<R>::value
        BOOST_PP_REPEAT(NUM_ARGS, AND_ARG, ~)
      ;
    static std::string object_method_signature(const std::string & name) {
      std::stringstream sstr;
      sstr << ASReturn<R>::return_type()
           << " " << name << "("
             BOOST_PP_REPEAT(NUM_ARGS, INSERT_ARG, ~)
           << ") const";
      return sstr.str();
    }
  };

  template <typename R BOOST_PP_ENUM_TRAILING_PARAMS(NUM_ARGS, typename A)>
  struct CallingConvention<R (__fastcall *)(BOOST_PP_ENUM_PARAMS(NUM_ARGS, A))> {
    CallingConvention() {
      AS_AngelScriptDoesntSupportFastCall( (R (__fastcall *)(BOOST_PP_ENUM_PARAMS(NUM_ARGS, A)))0 );
    }
    operator asDWORD() const;
  };
#endif