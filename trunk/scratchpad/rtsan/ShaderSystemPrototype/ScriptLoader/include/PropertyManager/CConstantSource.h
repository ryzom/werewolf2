#ifndef __CCONSTANT_SOURCE_H__
#define __CCONSTANT_SOURCE_H__

#include "ISource.h"

template <class T>
class CConstantSource : public ISource<T> {
public:
	T* getValue() { return m_value; }

	unsigned int getSize() { return m_size; }

	CConstantSource(T* value, unsigned int size = 1) : m_size(std::min<unsigned int>(4, size)) {
		unsigned int length = sizeof(T)*m_size;
		m_value = new T[m_size];
		memcpy(m_value, value, length);
	}

	CConstantSource(T value, unsigned int size = 1) : m_size(std::min<unsigned int>(4, size)) {
		unsigned int length = sizeof(T)*m_size;
		m_value = new T[m_size];
		memcpy(m_value, &value, length);
	}

	~CConstantSource() {
		delete[] m_value;
	}

private:
	T* m_value;
	unsigned int m_size;
};

#endif // __CCONSTANT_SOURCE_H__