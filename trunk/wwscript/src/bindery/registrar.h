#ifndef REGISTRAR_H
#define REGISTRAR_H

#include "object_proxy.h"
#include "object_methods.h"
#include <cassert>

template <typename T, bool use_proxy>
struct RegistrarImpl {
  typedef ObjectProxy<T> ObjectType;
};

template <typename T>
struct RegistrarImpl<T, false> {
  typedef T ObjectType;
};

template <typename T>
struct RegistrarWorker
  : RegistrarImpl<T, !ASTraits<T>::has_ref_cnt && ASTraits<T>::handle> {
  static const int type_flags 
    = boost::is_floating_point<ObjectType>::value ? asOBJ_FLOAT : (
        boost::is_class<ObjectType>::value ? (
          asOBJ_CLASS | (boost::has_trivial_constructor<ObjectType>::value ? 0 : asOBJ_CLASS_CONSTRUCTOR)
                      | (boost::has_trivial_destructor<ObjectType>::value ? 0 : asOBJ_CLASS_DESTRUCTOR)
                      | (boost::has_trivial_assign<ObjectType>::value ? 0 : asOBJ_CLASS_ASSIGNMENT)
        ) : asOBJ_PRIMITIVE
      );

  static void * __cdecl allocate(size_t size) {
    return ObjectType::operator new(size);
  }
  
  static void __cdecl deallocate(void * ptr) {
    return ObjectType::operator delete(ptr);
  }
  
  static void __cdecl construct(ObjectType * address) {
    ::new (address) ObjectType();
  }
  
  static void __cdecl destroy(ObjectType * address) {
    address->~ObjectType();
  }
  
  static ObjectType & __cdecl assign(ObjectType & lhs, const ObjectType & rhs) {
    return lhs = rhs;
  }
  
  static void __cdecl addref(ObjectType * ptr) {
    intrusive_ptr_add_ref(ptr);
  }
  
  static void __cdecl release(ObjectType * ptr) {
    intrusive_ptr_release(ptr);
  }

};

template <typename T, bool create>
struct ObjectSize : boost::integral_constant<size_t, 0> {};
template <typename T>
struct ObjectSize<T, true> : boost::integral_constant<size_t, sizeof(T)> {};

template <typename T, bool perform>
struct Switch {
  static void reg_constructor(asIScriptEngine * engine) {
    int r = engine->REGISTER_OBJECT_BEHAVIOUR(T, asBEHAVE_CONSTRUCT, &RegistrarWorker<T>::construct);
    assert(r >= 0);
  }
  static void reg_destructor(asIScriptEngine * engine) {
    int r = engine->REGISTER_OBJECT_BEHAVIOUR(T, asBEHAVE_DESTRUCT, &RegistrarWorker<T>::destroy);
    assert(r >= 0);
  }
  static void reg_assignment(asIScriptEngine * engine) {
    int r = engine->REGISTER_OBJECT_BEHAVIOUR(T, asBEHAVE_ASSIGNMENT, &RegistrarWorker<T>::assign);
    assert(r >= 0);
  }
  static void reg_addref(asIScriptEngine * engine) {
    int r = engine->REGISTER_OBJECT_BEHAVIOUR(T, asBEHAVE_ADDREF, &RegistrarWorker<T>::addref);
    assert(r >= 0);
  }
  static void reg_release(asIScriptEngine * engine) {
    int r = engine->REGISTER_OBJECT_BEHAVIOUR(T, asBEHAVE_RELEASE, &RegistrarWorker<T>::release);
    assert(r >= 0);
  }
  static void reg_alloc(asIScriptEngine * engine) {
    int r = engine->RegisterObjectBehaviour(ASTraits<T>::get_name().c_str(),
                                            asBEHAVE_ALLOC,
                                            (ASTraits<T>::get_name() + " & f(uint)").c_str(),
                                            asFUNCTION(&RegistrarWorker<T>::allocate),
                                            asCALL_CDECL);
    assert(r >= 0);
  }
  static void reg_free(asIScriptEngine * engine) {
    int r = engine->RegisterObjectBehaviour(ASTraits<T>::get_name().c_str(),
                                            asBEHAVE_FREE,
                                            ("void f(" + ASTraits<T>::get_name() + " & in)").c_str(),
                                            asFUNCTION(&RegistrarWorker<T>::deallocate),
                                            asCALL_CDECL);
    assert(r >= 0);
  }
};
template <typename T>
struct Switch<T, false> {
  static void reg_constructor(asIScriptEngine *) {}
  static void reg_destructor(asIScriptEngine *) {}
  static void reg_assignment(asIScriptEngine *) {}
  static void reg_addref(asIScriptEngine *) {}
  static void reg_release(asIScriptEngine *) {}
  static void reg_alloc(asIScriptEngine *) {}
  static void reg_free(asIScriptEngine *) {}
};

template <typename T, asDWORD flags = RegistrarWorker<T>::type_flags>
struct Registrar {
  static const bool do_construct
    = (flags & asOBJ_CLASS_CONSTRUCTOR) && ASTraits<T>::create;
  static const bool do_destroy
    = (flags & asOBJ_CLASS_DESTRUCTOR) && ASTraits<T>::create && !ASTraits<T>::handle;
  static const bool do_assign
    = (flags & asOBJ_CLASS_ASSIGNMENT) && ASTraits<T>::create;
  static const bool do_addref = ASTraits<T>::handle;
  static const bool do_release = ASTraits<T>::handle;
  static const bool do_alloc = ASTraits<T>::create && ASTraits<T>::handle;
  static const bool do_free = ASTraits<T>::create && ASTraits<T>::handle;
        
  static void register_type(asIScriptEngine * engine) {
    typedef RegistrarWorker<T> Worker;
    engine->RegisterObjectType(ASTraits<T>::get_name().c_str(), 
                               ObjectSize<typename Worker::ObjectType, ASTraits<T>::create>(), 
                               flags);
    Switch<T, do_construct>::reg_constructor(engine);
    Switch<T, do_destroy>::reg_destructor(engine);
    Switch<T, do_assign>::reg_assignment(engine);
    Switch<T, do_addref>::reg_addref(engine);
    Switch<T, do_release>::reg_release(engine);
    Switch<T, do_alloc>::reg_alloc(engine);
    Switch<T, do_free>::reg_free(engine);
  }
};

#define REGISTER_TYPE(type, engine) Registrar<type>::register_type(engine)

#endif