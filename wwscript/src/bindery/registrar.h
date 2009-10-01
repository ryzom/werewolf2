#ifndef REGISTRAR_H
#define REGISTRAR_H

#ifdef NL_OS_UNIX
#define __cdecl
#define __stdcall __attribute__ ((stdcall))
#define __fastcall __attribute__((fastcall))
#endif

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

#define REGISTRAR_IMPL RegistrarImpl<T, !ASTraits<T>::has_ref_cnt && ASTraits<T>::handle>

template <typename T>
struct RegistrarWorker
  : REGISTRAR_IMPL {

  static const bool has_constructor = !boost::has_trivial_constructor<typename REGISTRAR_IMPL::ObjectType>::value;
  static const bool has_destructor  = !boost::has_trivial_destructor<typename REGISTRAR_IMPL::ObjectType>::value;
  static const bool has_assign      = !boost::has_trivial_assign<typename REGISTRAR_IMPL::ObjectType>::value;
  static const bool is_class        = boost::is_class<typename REGISTRAR_IMPL::ObjectType>::value;
  static const bool is_float        = boost::is_floating_point<typename REGISTRAR_IMPL::ObjectType>::value;
  static const bool is_obj_pod      = (!has_constructor || !has_destructor);

  static const int type_flags = (ASTraits<T>::objref) ? asOBJ_REF |(!ASTraits<T>::handle?asOBJ_NOHANDLE:0) : 
				asOBJ_VALUE | (is_float ? asOBJ_APP_FLOAT :
  				(is_class ? asOBJ_APP_CLASS | 
						(has_constructor ? asOBJ_APP_CLASS_CONSTRUCTOR : 0) |
                                                (has_destructor ? asOBJ_APP_CLASS_DESTRUCTOR : 0) |
						(has_assign ? asOBJ_APP_CLASS_ASSIGNMENT : 0) |
						(is_obj_pod ? asOBJ_POD : 0) : asOBJ_POD));


/*
  static const int type_flags 
    = boost::is_floating_point<typename REGISTRAR_IMPL::ObjectType>::value ? (asOBJ_VALUE|asOBJ_APP_FLOAT) : (
        boost::is_class<typename REGISTRAR_IMPL::ObjectType>::value ? (
          asOBJ_APP_CLASS | (has_constructor ? asOBJ_APP_CLASS_CONSTRUCTOR : 0)
                      | (has_destructor ? asOBJ_APP_CLASS_DESTRUCTOR : 0)
                      | (has_assign ? asOBJ_APP_CLASS_ASSIGNMENT : 0)
                      | (is_obj_value ? (is_obj_pod ? asOBJ_VALUE|asOBJ_POD : asOBJ_VALUE) : asOBJ_REF)
        ) : (asOBJ_VALUE|asOBJ_APP_PRIMITIVE)
      );
 */

  static int getTypeFlags() {
    int flags;
    if(ASTraits<T>::has_ref_cnt) {
      flags = asOBJ_REF;
    } else {
      flags = asOBJ_VALUE;
      if(is_float) flags = flags | asOBJ_APP_FLOAT;
      else if(is_class) {
        flags = flags | asOBJ_APP_CLASS;
	if(has_constructor) flags = flags | asOBJ_APP_CLASS_CONSTRUCTOR;
        if(has_destructor) flags = flags | asOBJ_APP_CLASS_DESTRUCTOR;
        if(has_assign) flags = flags | asOBJ_APP_CLASS_ASSIGNMENT;
        if(is_obj_pod) flags = flags | asOBJ_POD;
      } else {
        flags = flags | asOBJ_APP_PRIMITIVE; 
      }
    }
    return flags;
  }

  static void * __cdecl allocate(size_t size) {
    return REGISTRAR_IMPL::ObjectType::operator new(size);
  }
  
  static void __cdecl deallocate(void * ptr) {
    return REGISTRAR_IMPL::ObjectType::operator delete(ptr);
  }
  
  static void __cdecl construct(typename REGISTRAR_IMPL::ObjectType * address) {
    ::new (address) typename REGISTRAR_IMPL::ObjectType();
  }
  
  static void __cdecl destroy(typename REGISTRAR_IMPL::ObjectType * address) {
    //address->~ObjectType();
    delete address;
  }
  
  static typename REGISTRAR_IMPL::ObjectType 
		& __cdecl assign(
			typename REGISTRAR_IMPL::ObjectType & lhs, 
			const typename REGISTRAR_IMPL::ObjectType & rhs) {
    return lhs = rhs;
  }
  
  static void __cdecl addref(typename REGISTRAR_IMPL::ObjectType * ptr) {
    intrusive_ptr_add_ref(ptr);
  }
  
  static void __cdecl release(typename REGISTRAR_IMPL::ObjectType * ptr) {
    intrusive_ptr_release(ptr);
  }

  static typename REGISTRAR_IMPL::ObjectType * __cdecl factory() {
    return ::new typename REGISTRAR_IMPL::ObjectType();
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
  static void reg_factory(asIScriptEngine * engine) {
    int r = engine->RegisterObjectBehaviour(ASTraits<T>::get_name().c_str(),
						asBEHAVE_FACTORY,
						(ASTraits<T>::get_name() + " @ f()").c_str(),
						asFUNCTION(&RegistrarWorker<T>::factory),
						asCALL_CDECL);
    assert(r >= 0);
  }
  /* AngelScript no longer supports this */
  /*
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
   */
};
template <typename T>
struct Switch<T, false> {
  static void reg_constructor(asIScriptEngine *) {}
  static void reg_destructor(asIScriptEngine *) {}
  static void reg_assignment(asIScriptEngine *) {}
  static void reg_addref(asIScriptEngine *) {}
  static void reg_release(asIScriptEngine *) {}
  static void reg_factory(asIScriptEngine *) {}
  /* AngelScript no longer supports this */
  /*
  static void reg_alloc(asIScriptEngine *) {}
  static void reg_free(asIScriptEngine *) {}
   */
};

template <typename T, asDWORD flags = RegistrarWorker<T>::type_flags>
struct Registrar {
  static const bool do_construct
    = ((flags & asOBJ_APP_CLASS_CONSTRUCTOR) && ASTraits<T>::create);
  static const bool do_destroy
    = (flags & asOBJ_APP_CLASS_DESTRUCTOR) && ASTraits<T>::create && !ASTraits<T>::handle;
  static const bool do_assign
    = (flags & asOBJ_APP_CLASS_ASSIGNMENT) && ASTraits<T>::create;
  static const bool do_addref = (ASTraits<T>::handle || ASTraits<T>::has_ref_cnt || ASTraits<T>::objref);
  static const bool do_release = (ASTraits<T>::handle || ASTraits<T>::has_ref_cnt || ASTraits<T>::objref);
  static const bool do_factory = (ASTraits<T>::create&&ASTraits<T>::objref);
  /* AngelScript no longer supports this. */
  /*
  static const bool do_alloc = ASTraits<T>::create && ASTraits<T>::handle;
  static const bool do_free = ASTraits<T>::create && ASTraits<T>::handle;
   */
        
  static void register_type(asIScriptEngine * engine) {
    typedef RegistrarWorker<T> Worker;
    engine->RegisterObjectType(ASTraits<T>::get_name().c_str(), 
                               ObjectSize<typename Worker::ObjectType, ASTraits<T>::create>(), 
                               flags);
    //Switch<T, do_construct>::reg_constructor(engine);
    //Switch<T, do_destroy>::reg_destructor(engine);
    //Switch<T, do_assign>::reg_assignment(engine);
    Switch<T, do_addref>::reg_addref(engine);
    Switch<T, do_release>::reg_release(engine);
    Switch<T, do_factory>::reg_factory(engine);
    /* AngelScript no longer supports this */
    /*
    Switch<T, do_alloc>::reg_alloc(engine);
    Switch<T, do_free>::reg_free(engine);
     */
  }
};

#define REGISTER_TYPE(type, engine) Registrar<type>::register_type(engine)

#endif
