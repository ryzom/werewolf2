#ifndef __ISOURCE_H__
#define __ISOURCE_H__

#include <boost/python.hpp>
#include <memory>

template <class T>
class ISource {
public:
	virtual T* getValue() = 0;
	virtual unsigned int getSize() = 0;
};

template <class T>
class ISourceWrap;

// TODO henri:henri possibly a hack. Investigate nicer methods.
template <class T>
boost::python::list getSourceValue(ISource<T>* source) {
	using namespace boost::python;
	using namespace boost::python::api;
	list list;
	for(unsigned int i = 0; i < source->getSize(); ++i) {
		list.append(source->getValue()[i]);
	}

	return list;
}

template <class T>
class ISourceWrap : public ISource<T>, public boost::python::wrapper< ISource<T> > {
public:
	T* getValue() {
        return boost::python::call<T*>(this->get_override("getValue").ptr());
	}

	unsigned int getSize() {
		return boost::python::call<unsigned int>(this->get_override("getSize").ptr());
	}

	static void init_script_class(const char* typeName) {
		using namespace boost::python;
		using namespace boost::python::api;
		class_< ISourceWrap<T>, boost::noncopyable > c(typeName, no_init);
		c.def("getValue", &getSourceValue<T>);
		c.def("getSize", pure_virtual(&ISource<T>::getSize));
	}
};

#endif // __ISOURCE_H__