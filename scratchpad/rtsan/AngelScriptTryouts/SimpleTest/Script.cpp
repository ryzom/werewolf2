#include "Script.h"
#include "ScriptManager.h"
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <iostream>
#include <io.h>
#include <angelscript.h>

XERCES_CPP_NAMESPACE_USE

Script::Script(std::string file, const char* section) : m_xmlfile(file), m_section(section) {
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
		exit(-1);
	} catch (const DOMException& toCatch) {
		char* message = XMLString::transcode(toCatch.msg);
		std::cout << "Exception message is: \n"
			<< message << "\n";
		XMLString::release(&message);
		exit(-1);
	} catch (...) {
		std::cout << "Unexpected Exception \n" ;
		exit(-1);
	}

	// parse the script node
	parseScript((DOMNode*)doc->getDocumentElement());
	parser->release();
}

Script::~Script() {
	for(functionMap::iterator iter = m_functions.begin(); iter != m_functions.end(); iter++) {
		delete iter->second;
	}
	m_functions.clear();
}

const ScriptFunction* Script::getFunction(const char* name) const {
	Script::functionMap::const_iterator iter = m_functions.find(name);
	if(iter == m_functions.end())
		return NULL;
	return iter->second;
}

const std::string& Script::getName() const {
	return m_name;
}

void Script::parseScript(DOMNode* node) {
	if(!node)
		return;
	if(node->getNodeType() != DOMNode::ELEMENT_NODE)
		return;

	char *name = XMLString::transcode(node->getNodeName());
	if(strcmp(name, "script") != 0)
		return;

	DOMNamedNodeMap* attributes = node->getAttributes();
	DOMAttr* attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("name"));
	m_name = XMLString::transcode(attr->getValue());
	attr = (DOMAttr*) attributes->getNamedItem(XMLString::transcode("src"));
	m_scriptfile = XMLString::transcode(attr->getValue());

	// load the actual script
	FILE *f = fopen(m_scriptfile.c_str(), "rb");
	int length = _filelength(_fileno(f));
	char *script = new char[length];
	fread(script, length, 1, f);
	fclose(f);

	asIScriptEngine* engine = ScriptManager::instance().getEngine();

	// Compile the script
	engine->AddScriptSection(m_name.c_str(), m_section, script, length);
	engine->Build(m_name.c_str());

	// Add functions
	for(DOMNode* child = node->getFirstChild(); child != 0; child=child->getNextSibling()) {
		if(child->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;
		name = XMLString::transcode(child->getNodeName());
		if(strcmp(name, "function") == 0) {
			addFunction(child);
		}
	}
}

void Script::addFunction(DOMNode* node) {
	ScriptFunction* func = new ScriptFunction(node, this);
	m_functions[func->getName().c_str()] =  func;
}

Script::functionMap::const_iterator Script::begin() const {
	return m_functions.begin();
}

Script::functionMap::const_iterator Script::end() const {
	return m_functions.end();
}