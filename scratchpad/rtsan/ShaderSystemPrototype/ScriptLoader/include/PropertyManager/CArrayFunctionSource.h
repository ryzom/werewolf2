#ifndef __CARRAY_FUNCTION_SOURCE_H__
#define __CARRAY_FUNCTION_SOURCE_H__

#include "IArraySource.h"

template <class T>
class CArrayFunctionSource : public IArraySource<T> {
public:
	T* getValue(int index) { return (*m_func)(index); }

	unsigned int getSize() { return m_size; }

	CArrayFunctionSource(T* (*func)(int), unsigned int size = 1) : m_size(std::min<unsigned int>(4, size)), m_func(func) {;}

	~CArrayFunctionSource() {
		// does nothing
	}

private:
	T* (*m_func)(int);
	unsigned int m_size;
};

#endif // __CFUNCTION_SOURCE_H__