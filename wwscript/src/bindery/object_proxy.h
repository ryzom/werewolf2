#if !BOOST_PP_IS_ITERATING
  #ifndef OBJECT_PROXY_H
  #define OBJECT_PROXY_H
  
  #include "object_proxy_base.h"

  template <typename T>
  class ObjectProxy : ObjectProxyBase<T> {
    public:
      ObjectProxy() {}
      ObjectProxy(const ObjectProxy & other) : ObjectProxyBase<T>(other) {}
      ObjectProxy & operator=(const ObjectProxy & rhs) {
        object = rhs.object;
        return *this;
      }

      #define BOOST_PP_ITERATION_PARAMS_1 (3, (0, MAX_ARGS, "object_proxy.h"))
      #include BOOST_PP_ITERATE()

      using ObjectProxyBase<T>::object;
      
      void add_ref(void) {
        ++ref_cnt;
      }
      void release(void) {
        if (--ref_cnt == 0) {
          delete this;
        }
      }

      void * operator new(size_t size) {
        return ::operator new(size);
      }
      void operator delete(void * ptr) {
        ::operator delete(ptr);
      }
    private:
      void * operator new[](size_t size);
      void operator delete[](void * ptr);
  };
  
  template <typename T>
  void intrusive_ptr_add_ref(ObjectProxy<T> * ptr) {
    ptr->add_ref();
  }
  template <typename T>
  void intrusive_ptr_release(ObjectProxy<T> * ptr) {
    ptr->release();
  }
  #endif
#elif BOOST_PP_ITERATION_DEPTH() == 1
  #define NUM_ARGS BOOST_PP_FRAME_ITERATION(1)
  #if NUM_ARGS != 0
    template <BOOST_PP_ENUM_PARAMS(NUM_ARGS, typename A)>
    ObjectProxy(BOOST_PP_ENUM_BINARY_PARAMS(NUM_ARGS, A, a))
      : ObjectProxyBase<T>(BOOST_PP_ENUM_PARAMS(NUM_ARGS, a)) {}
  #endif
#endif
