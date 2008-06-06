#ifndef __CARRAY_MEMBER_FUNCTION_SOURCE_H__
#define __CARRAY_MEMBER_FUNCTION_SOURCE_H__

#include "IArraySource.h"

template <class T, class P>
class CArrayMemberFunctionSource : public IArraySource<T> {
public:
	typedef T* (P::*memberFunction) (int);

	T* getValue(int index) { return (m_object->*m_func)(index); }

	unsigned int getSize() { return m_size; }

	CArrayMemberFunctionSource(memberFunction func, P* object, unsigned int size = 1) :
		m_size(std::min<unsigned int>(4, size)), m_func(func), m_object(object) {;}

	~CArrayMemberFunctionSource() {
		// does nothing
	}

private:
	memberFunction m_func;
	P* m_object;
	unsigned int m_size;
};

#endif // __CARRAY_MEMBER_FUNCTION_SOURCE_H__