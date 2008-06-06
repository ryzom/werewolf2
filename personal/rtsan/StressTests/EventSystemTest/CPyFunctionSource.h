#ifndef __CPYFUNCTION_SOURCE_H__
#define __CPYFUNCTION_SOURCE_H__

#include "ISource.h"
#include <boost/python.hpp>
#include <memory>

template <class T>
class CPyFunctionSource : public ISource<T> {
public:
	T* getValue() { m_result = boost::python::call<T>(m_func); return &m_result; }

	unsigned int getSize() { return 1; }

	CPyFunctionSource(boost::python::object func) : m_func(func.ptr()) {;}

	~CPyFunctionSource() {
		// does nothing
	}

	static void init_script_class(const char* typeName) {
		using namespace boost::python;
		using namespace boost::python::api;
		class_< CPyFunctionSource<T>, bases< ISource<T> > > c(typeName, init<object>());
	}

private:
	PyObject* m_func;
	T m_result;
};

#endif // __CFUNCTION_SOURCE_H__
