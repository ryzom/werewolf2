#ifndef __IARRAY_SOURCE_H__
#define __IARRAY_SOURCE_H__

template <class T>
class IArraySource {
public:
	virtual T* getValue(int index) = 0;
	virtual unsigned int getSize() = 0;
};

#endif // __IARRAY_SOURCE_H__