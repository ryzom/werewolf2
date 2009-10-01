#ifndef TRAITS_H
#define TRAITS_H

#include <string>
#include <boost/type_traits.hpp>
#include "angelscript.h"

template <typename T> class ObjectProxy;

template <typename T>
struct ASTraits {
  static const bool registered = false;
  static const bool handle = false;
  static const bool create = false;
  static const bool has_ref_cnt = false;
};

template <typename T>
struct ASTraits<ObjectProxy<T> > {
  static const bool registered = ASTraits<T>::registered;
  static const bool handle = ASTraits<T>::handle;
  static const bool create = ASTraits<T>::create;
  static const bool has_ref_cnt = ASTraits<T>::has_ref_cnt;
  static std::string get_name(void) {
    return ASTraits<T>::get_name();
  }
};

#define ASTRAITS(type, name, allow_handles, allow_script_create, want_objref) \
  template <>                                                      \
  struct ASTraits<type> {                                          \
    static const bool registered = true;                           \
    static const bool handle = (want_objref?allow_handles:false);  \
    static const bool create = allow_script_create;                \
    static const bool has_ref_cnt = false;                         \
    static const bool objref = want_objref;                        \
    static std::string get_name(void) {                            \
      return name;                                                 \
    }                                                              \
  }

#define ASTRAITS_REFCNT(type, name, allow_handles, allow_script_create) \
  template <>                                         \
  struct ASTraits<type> {                             \
    static const bool registered = true;              \
    static const bool handle = allow_handles;         \
    static const bool create = allow_script_create;   \
    static const bool has_ref_cnt = true;             \
    static const bool objref = true;                  \
    static std::string get_name(void) {               \
      return name;                                    \
    }                                                 \
  }


ASTRAITS(void, "void", false, true, false);
ASTRAITS(bool, "bool", false, true, false);
ASTRAITS(signed char, "int8", false, true, false);
ASTRAITS(signed short, "int16", false, true, false);
ASTRAITS(signed long, "int", false, true, false);
ASTRAITS(signed long long, "int64", false, true, false);
ASTRAITS(unsigned char, "uint8", false, true, false);
ASTRAITS(unsigned short, "uint16", false, true, false);
ASTRAITS(unsigned long, "uint", false, true, false);
ASTRAITS(unsigned long long, "uint64", false, true, false);
ASTRAITS(float, "float", false, true, false);
ASTRAITS(double, "double", false, true, false);
ASTRAITS(signed int, "int", false, true, false);
ASTRAITS(unsigned int, "uint", false, true, false);
ASTRAITS(long double, "double", false, true, false);
ASTRAITS(wchar_t, "int16", false, true, false);
#if CHAR_MAX == UCHAR_MAX
  ASTRAITS(char, "uint8", false, true, false);
#else
  ASTRAITS(char, "int8", false, true, false);
#endif
ASTRAITS(std::string, "string", false, true, false);

template <typename T>
struct ASArg
  : boost::integral_constant<bool, ASTraits<T>::registered && 
                                  !ASTraits<T>::handle && 
                                   ASTraits<T>::create> {
  static std::string arg_type(void) {
    return ASTraits<T>::get_name();
  }
};

template <typename T>
struct ASArg<T &> : boost::integral_constant<bool, ASTraits<T>::registered> {
  static std::string arg_type(void) {
    return ASTraits<T>::get_name() + " & inout";
  }
};

template <typename T>
struct ASArg<const T &> : boost::integral_constant<bool, ASTraits<T>::registered> {
  static std::string arg_type(void) {
    return "const " + ASTraits<T>::get_name() + " & in";
  }
};

template <typename T>
struct ASArg<T *&>
  : boost::integral_constant<bool, ASTraits<T>::registered && ASTraits<T>::handle> {
  static std::string arg_type(void) {
    return ASTraits<T>::get_name() + "@& inout";
  }
};

template <typename T>
struct ASArg<const T *&>
  : boost::integral_constant<bool, ASTraits<T>::registered && ASTraits<T>::handle> {
  static std::string arg_type(void) {
    return "const " + ASTraits<T>::get_name() + "@& in";
  }
};

template <typename T>
struct ASArg<T **>
  : boost::integral_constant<bool, ASTraits<T>::registered && ASTraits<T>::handle> {
  static std::string arg_type(void) {
    return ASTraits<T>::get_name() + "@& inout";
  }
};

template <typename T>
struct ASArg<const T **>
  : boost::integral_constant<bool, ASTraits<T>::registered && ASTraits<T>::handle> {
  static std::string arg_type(void) {
    return "const " + ASTraits<T>::get_name() + "@& in";
  }
};

// ------------------------------------------------------------------------------

template <typename T>
struct ASReturn
  : boost::integral_constant<bool, ASTraits<T>::registered &&
                                  !ASTraits<T>::handle && 
                                   ASTraits<T>::create> {
  static std::string return_type(void) {
    return ASTraits<T>::get_name();
  }
};

template <typename T>
struct ASReturn<T &> : boost::integral_constant<bool, ASTraits<T>::registered> {
  static std::string return_type(void) {
    return ASTraits<T>::get_name() + " &";
  }
};

template <typename T>
struct ASReturn<const T &> : boost::integral_constant<bool, ASTraits<T>::registered> {
  static std::string return_type(void) {
    return "const " + ASTraits<T>::get_name() + " &";
  }
};

template <typename T>
struct ASReturn<T *&>
  : boost::integral_constant<bool, ASTraits<T>::registered && ASTraits<T>::handle> {
  static std::string return_type(void) {
    return ASTraits<T>::get_name() + "@&";
  }
};

template <typename T>
struct ASReturn<const T *&>
  : boost::integral_constant<bool, ASTraits<T>::registered && ASTraits<T>::handle> {
  static std::string return_type(void) {
    return "const " + ASTraits<T>::get_name() + "@&";
  }
};

template <typename T>
struct ASReturn<T **>
  : boost::integral_constant<bool, ASTraits<T>::registered && ASTraits<T>::handle> {
  static std::string return_type(void) {
    return ASTraits<T>::get_name() + "@&";
  }
};

template <typename T>
struct ASReturn<const T **>
  : boost::integral_constant<bool, ASTraits<T>::registered && ASTraits<T>::handle> {
  static std::string return_type(void) {
    return "const " + ASTraits<T>::get_name() + "@&";
  }
};

template <typename T>
struct ASReturn<T *>
 : boost::integral_constant<bool, ASTraits<T>::registered && ASTraits<T>::handle> {
  static std::string return_type(void) {
    return ASTraits<T>::get_name() + "@";
  }
};

#endif
