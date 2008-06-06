#ifndef __CVARIABLE_SOURCE_H__
#define __CVARIABLE_SOURCE_H__

#include "ISource.h"

template <class T>
class CVariableSource : public ISource<T> {
public:
	T* getValue() { return m_value; }

	unsigned int getSize() { return m_size; }

	CVariableSource(T* value, unsigned int size = 1) : m_size(std::min<unsigned int>(4, size)), m_value(value) {;}

	~CVariableSource() {
		// do nothing
	}

private:
	T* m_value;
	unsigned int m_size;
};

#endif // __CVARIABLE_SOURCE_H__