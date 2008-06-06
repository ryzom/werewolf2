#ifndef __CBASIC_PROPERTY_H__
#define __CBASIC_PROPERTY_H__

#include "ISource.h"
#include "IProperty.h"
#include "CConstantSource.h"
#include "ScriptLoader/ScriptVariable.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"


template <class T>
class CBasicProperty : public IProperty {
private:
	ISource<T>* m_value;

public:
	CBasicProperty(ISource<T>* value, const char* name);
	virtual ~CBasicProperty();

	void setCGGLParameter(CGparameter& parameter);
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);

	T* getValue() { return m_value->getValue(); };
	void getHash(std::stringstream& hash);
	static IProperty* create(xercesc::DOMNode* node);
};

template <class T>
CBasicProperty<T>::CBasicProperty(ISource<T>* value, const char* name) : m_value(value) {
	setName(name);
}

template <class T>
CBasicProperty<T>::~CBasicProperty() {
	delete m_value;
}

template <class T>
void CBasicProperty<T>::setCGGLParameter(CGparameter &parameter) {
	float *val = new float[m_value->getSize()];
	for(unsigned int i = 0; i < m_value->getSize(); i++) {
		val[i] = (float)((m_value->getValue())[i]);
	}
	switch(m_value->getSize()) {
	case 1:
		cgGLSetParameter1fv(parameter, val);
		break;
	case 2:
		cgGLSetParameter2fv(parameter, val);
		break;
	case 3:
		cgGLSetParameter3fv(parameter, val);
		break;
	case 4:
		cgGLSetParameter4fv(parameter, val);
		break;
	default:
		printf("Ooops - wrong size for property.");
	}

	delete[] val;
}

template <class T>
void CBasicProperty<T>::setScriptParameter(ScriptVariable *var) {
	// TODO: IMPLEMENT THIS DECENTLY
	var->setValue(*(m_value->getValue()));
}

template <class T>
void CBasicProperty<T>::getHash(std::stringstream &hash) {
	unsigned int length = sizeof(T)*m_value->getSize();
	char* t = new char[length];
	memcpy(t, m_value->getValue(), length);
	for(unsigned int i = 0; i < length; i++)
		hash << t[i];
	delete[] t;
}

template <class T>
IProperty* CBasicProperty<T>::create(xercesc::DOMNode* node) {
	xercesc::DOMNamedNodeMap* attributes = node->getAttributes();
	xercesc::DOMAttr* attr = (xercesc::DOMAttr*)attributes->getNamedItem(xercesc::XMLString::transcode("name"));
	const char* name = xercesc::XMLString::transcode(attr->getValue());
	
	xercesc::DOMNodeList* children = node->getChildNodes();
	unsigned int size = (unsigned int)children->getLength();
	T* value = new T[size];
	unsigned int real_size = 0;

	for(unsigned int i = 0; i < size; i++) {
		xercesc::DOMNode* child = children->item(i);
		if(child->getNodeType() != xercesc::DOMNode::ELEMENT_NODE)
			continue;
		char* value_name = xercesc::XMLString::transcode(child->getNodeName());
		if(strcmp(value_name, "value") == 0) {
			value[real_size] = (T)atof(xercesc::XMLString::transcode(child->getTextContent()));
		}
		real_size++;
		if(real_size == 4)
			break;
	}

	IProperty* ret;
	if(real_size < 1)
		ret =  NULL;
	else
		ret = new CBasicProperty<T>(new CConstantSource<T>(value, real_size), name);
	delete[] value;
	return ret;
}

#endif // __CBASIC_PROPERTY_H__