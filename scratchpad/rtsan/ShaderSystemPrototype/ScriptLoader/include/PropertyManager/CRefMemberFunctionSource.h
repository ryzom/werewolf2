#ifndef __CREF_MEMBER_FUNCTION_SOURCE_H__
#define __CREF_MEMBER_FUNCTION_SOURCE_H__

#include "ISource.h"

template <class T, class P>
class CRefMemberFunctionSource : public ISource<T> {
public:
	typedef const T& (P::*memberFunction) (void) const;

	T* getValue() { return (T*)&(m_object->*m_func)(); }

	unsigned int getSize() { return m_size; }

	CRefMemberFunctionSource(memberFunction func, P* object, unsigned int size = 1) :
		m_size(std::min<unsigned int>(4, size)), m_func(func), m_object(object) {;}

	~CRefMemberFunctionSource() {
		// does nothing
	}

private:
	memberFunction m_func;
	P* m_object;
	unsigned int m_size;
};

#endif // __CREF_MEMBER_FUNCTION_SOURCE_H__