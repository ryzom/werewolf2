#ifndef OBJECT_METHODS_H
#define OBJECT_METHODS_H

#include "calling_convention.h"
#include "global_function.h"
#include <boost/utility/enable_if.hpp>

void AS_StdcallFunctionsCannotBeObjectMethods(Undefined &);

template <typename T, typename Fn, typename D = void>
struct ObjectMethod {
  ObjectMethod() {
    CallingConvention<Fn>();
  }
  
  static std::string signature(const std::string & name);
  static asUPtr convert(Fn);
  static const asDWORD calling_convention;
};

template <typename T, typename Fn>
struct ObjectMethod<T, Fn,
                     typename boost::enable_if<
                       boost::integral_constant<bool,
                         (CallingConvention<Fn>::value == asCALL_THISCALL)
                       >
                     >::type>
  : boost::integral_constant<bool,
      boost::is_base_of<typename CallingConvention<Fn>::ObjectType, T>::value &&
      CallingConvention<Fn>::valid_object_method> {
  static std::string signature(const std::string & name) {
    return CallingConvention<Fn>::object_method_signature(name);
  }
  
  static const asDWORD calling_convention = asCALL_THISCALL;
  
  static asUPtr convert(Fn fn) {
    return asSMethodPtr<sizeof(void (T::*)())>::Convert((void (T::*)())(fn));
  }
};

template <typename ClassType, typename T>
struct IsValidClassArgType : boost::false_type {};

template <typename ClassType, typename T>
struct IsValidClassArgType<ClassType, T *> : boost::is_base_of<T, ClassType> {};
template <typename ClassType, typename T>
struct IsValidClassArgType<ClassType, const T *> : boost::is_base_of<T, ClassType> {};
template <typename ClassType, typename T>
struct IsValidClassArgType<ClassType, T &> : boost::is_base_of<T, ClassType> {};
template <typename ClassType, typename T>
struct IsValidClassArgType<ClassType, const T &> : boost::is_base_of<T, ClassType> {};

template <typename ClassType, typename T>
struct IsValidClassArgType<ClassType, ObjectProxy<T> *> : boost::is_base_of<T, ClassType> {};
template <typename ClassType, typename T>
struct IsValidClassArgType<ClassType, const ObjectProxy<T> *> : boost::is_base_of<T, ClassType> {};
template <typename ClassType, typename T>
struct IsValidClassArgType<ClassType, ObjectProxy<T> &> : boost::is_base_of<T, ClassType> {};
template <typename ClassType, typename T>
struct IsValidClassArgType<ClassType, const ObjectProxy<T> &> : boost::is_base_of<T, ClassType> {};

template <typename T, typename Fn>
struct ObjectMethod<T, Fn,
                     typename boost::enable_if<
                       boost::integral_constant<bool,
                         (CallingConvention<Fn>::value == asCALL_CDECL)
                       >
                     >::type>
  : boost::integral_constant<bool,
    (IsValidClassArgType<T, typename CallingConvention<Fn>::FirstType>::value &&
     CallingConvention<Fn>::is_valid_firstcall_method) || 
    (IsValidClassArgType<T, typename CallingConvention<Fn>::LastType>::value &&
     CallingConvention<Fn>::is_valid_lastcall_method)>
{
  static std::string signature(const std::string & name) {
    return do_signature(name, boost::integral_constant<asDWORD, calling_convention>());
  }
  
  static std::string do_signature(const std::string & name,
                                  boost::integral_constant<asDWORD, asCALL_CDECL_OBJFIRST>) {
    return CallingConvention<Fn>::firstcall_signature(name);
  }
  static std::string do_signature(const std::string & name,
                                  boost::integral_constant<asDWORD, asCALL_CDECL_OBJLAST>) {
    return CallingConvention<Fn>::lastcall_signature(name);
  }

  static const asDWORD calling_convention
    = (IsValidClassArgType<T, typename CallingConvention<Fn>::FirstType>::value &&
       CallingConvention<Fn>::is_valid_firstcall_method) ? asCALL_CDECL_OBJFIRST 
                                                         : asCALL_CDECL_OBJLAST;

  static asUPtr convert(Fn fn) {
    return asFUNCTION(fn);
  }
};

template <typename T, typename Fn>
struct ObjectMethod<T, Fn,
                     typename boost::enable_if<
                       boost::integral_constant<bool,
                         (CallingConvention<Fn>::value == asCALL_STDCALL)
                       >
                     >::type> {
  ObjectMethod() {
    AS_StdcallFunctionsCannotBeObjectMethods(Fn());
  }
  
  static std::string signature(const std::string & name);
  static asUPtr convert(Fn);
  static const asDWORD calling_convention;
};


template <typename T, typename Fn>
std::string do_object_method_signature(Fn, const std::string & name, ObjectMethod<T, Fn> om) {
  AS_FunctionHasInvalidParameters(om);
  return ObjectMethod<T, Fn>::signature(name);
}

template <typename T, typename Fn>
std::string object_method_signature(Fn fn, const std::string & name) {
  return do_object_method_signature<T>(fn, name, ObjectMethod<T, Fn>());
}

template <typename T, typename Fn>
asUPtr convert_object_method(Fn fn) {
  return ObjectMethod<T, Fn>::convert(fn);
}

template <typename T, typename Fn>
asDWORD object_method_calling_conv(Fn) {
  return ObjectMethod<T, Fn>::calling_convention;
}


#define REGISTER_OBJECT_METHOD(type, name, fn)                             \
  RegisterObjectMethod( ASTraits<type>::get_name().c_str(),                \
                        object_method_signature<type>(fn, name).c_str(),   \
                        convert_object_method<type>(fn),                   \
                        object_method_calling_conv<type>(fn) )

#define REGISTER_OBJECT_BEHAVIOUR(type, behaviour, fn)                       \
  RegisterObjectBehaviour( ASTraits<type>::get_name().c_str(),               \
                           behaviour,                                        \
                           object_method_signature<type>(fn, "f").c_str(),   \
                           convert_object_method<type>(fn),                  \
                           object_method_calling_conv<type>(fn) )

#endif