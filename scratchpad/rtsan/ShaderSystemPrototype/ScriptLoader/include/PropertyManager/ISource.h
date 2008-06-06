#ifndef __ISOURCE_H__
#define __ISOURCE_H__

template <class T>
class ISource {
public:
	virtual T* getValue() = 0;
	virtual unsigned int getSize() = 0;
};

#endif // __ISOURCE_H__