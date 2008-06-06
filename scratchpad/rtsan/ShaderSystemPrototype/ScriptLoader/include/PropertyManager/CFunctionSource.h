#ifndef __CFUNCTION_SOURCE_H__
#define __CFUNCTION_SOURCE_H__

#include "ISource.h"

template <class T>
class CFunctionSource : public ISource<T> {
public:
	T* getValue() { return (*m_func)(); }

	unsigned int getSize() { return m_size; }

	CFunctionSource(T* (*func)(void), unsigned int size = 1) : m_size(std::min<unsigned int>(4, size)), m_func(func) {;}

	~CFunctionSource() {
		// does nothing
	}

private:
	T* (*m_func)(void);
	unsigned int m_size;
};

#endif // __CFUNCTION_SOURCE_H__