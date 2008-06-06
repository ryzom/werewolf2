// Local includes
#include "PropertyManager/PropertyMap.h"
#include "PropertyManager/PropertyFactory.h"

// Global includes
#include <stdio.h>
#include <iostream>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/util/PlatformUtils.hpp>

XERCES_CPP_NAMESPACE_USE

PropertyMap::PropertyMap(std::string file) {
	loadXML(file);
}

void PropertyMap::registerProperty(IProperty* property) {
	IProperty *old = getProperty(property->getName());
	if(old)
		delete old;
	m_propertyMap[property->getName()] = property;
}

void PropertyMap::removeProperty(const char* name) {
	propMap::const_iterator iter;
	iter = m_propertyMap.find(name);
	if(iter != m_propertyMap.end()) {
		delete iter->second;
	}
	m_propertyMap.erase(name);
}

IProperty* PropertyMap::getProperty(const char* name) {
	propMap::const_iterator iter;
	iter = m_propertyMap.find(name);
	if(iter == m_propertyMap.end())
		return NULL;
	return iter->second;
}

PropertyMap::~PropertyMap() {
	propMap::iterator iter;
	for(iter = m_propertyMap.begin(); iter != m_propertyMap.end(); iter++) {
		delete iter->second;
	}
	m_propertyMap.clear();
}

void PropertyMap::traverseGLParameters(CGparameter parameter) {
	if(parameter == 0)
		return;
	static int i = 0;
	do {
		IProperty* property = getProperty(cgGetParameterName(parameter));
		if(property == NULL) {
			printf("Parameter \"%s\" not found in property map!\n", cgGetParameterName(parameter));
		} else {
			property->setCGGLParameter(parameter);
		}
	} while((parameter = cgGetNextParameter(parameter))!=0);
}

void PropertyMap::traverseD3DParameters(CGparameter parameter) {
	if(parameter == 0)
		return;
	static int i = 0;
	do {
		IProperty* property = getProperty(cgGetParameterName(parameter));
		if(property == NULL) {
			printf("Parameter \"%s\" not found in property map!\n", cgGetParameterName(parameter));
		} else {
			property->setCGD3DParameter(parameter);
		}
	} while((parameter = cgGetNextParameter(parameter))!=0);
}

void PropertyMap::loadXML(std::string file) {
	try {
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		std::cout << "Error during initialization! :\n"
			<< message << "\n";
		XMLString::release(&message);
		exit(-1);
	}

	static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
    DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
	DOMBuilder* parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = NULL;

	try {
		doc = parser->parseURI(file.c_str());
	} catch (const XMLException& toCatch) {
		char* message = XMLString::transcode(toCatch.getMessage());
		std::cout << "Exception message is: \n"
			<< message << "\n";
		XMLString::release(&message);
		return;
	} catch (const DOMException& toCatch) {
		char* message = XMLString::transcode(toCatch.msg);
		std::cout << "Exception message is: \n"
			<< message << "\n";
		XMLString::release(&message);
		return;
	} catch (...) {
		std::cout << "Unexpected Exception \n" ;
		return;
	}

	DOMNode* node = (DOMNode*)doc->getDocumentElement();

	if(!node)
		return;
	if(node->getNodeType() != DOMNode::ELEMENT_NODE)
		return;

	char *name = XMLString::transcode(node->getNodeName());
	if(strcmp(name, "PropertyMap") != 0)
		return;

	// Add properties
	for(DOMNode* child = node->getFirstChild(); child != 0; child=child->getNextSibling()) {
		if(child->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;
		name = XMLString::transcode(child->getNodeName());
		IProperty* prop = PropertyFactory::instance().create(name, child);
		if(prop)
			registerProperty(prop);
	}
	parser->release();
	XMLPlatformUtils::Terminate();
}