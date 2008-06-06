#ifndef __CMEMBER_FUNCTION_SOURCE_H__
#define __CMEMBER_FUNCTION_SOURCE_H__

#include "ISource.h"

template <class T, class P>
class CMemberFunctionSource : public ISource<T> {
public:
	typedef T* (P::*memberFunction) (void);

	T* getValue() { return (m_object->*m_func)(); }

	unsigned int getSize() { return m_size; }

	CMemberFunctionSource(memberFunction func, P* object, unsigned int size = 1) :
		m_size(std::min<unsigned int>(4, size)), m_func(func), m_object(object) {;}

	~CMemberFunctionSource() {
		// does nothing
	}

private:
	memberFunction m_func;
	P* m_object;
	unsigned int m_size;
};

#endif // __CMEMBER_FUNCTION_SOURCE_H__