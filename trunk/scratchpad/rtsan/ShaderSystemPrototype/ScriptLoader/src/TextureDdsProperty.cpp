#include "PropertyManager/TextureDdsProperty.h"
#include "ScriptLoader/ScriptVariable.h"
#include "ScriptLoaderGeneral/TextureManager.h"

XERCES_CPP_NAMESPACE_USE

TextureDdsProperty::TextureDdsProperty(const char* file, const char* name) {
	setName(name);

	m_file = file;

	m_value = TextureManager::instance().loadTexture2D(file);
}

TextureDdsProperty::~TextureDdsProperty() {
	glDeleteTextures(1, &m_value);
}

void TextureDdsProperty::setCGGLParameter(CGparameter& parameter) {
	cgGLSetTextureParameter(parameter, m_value);
}

void TextureDdsProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME
}

void TextureDdsProperty::getHash(std::stringstream& hash) {
	hash << m_file;
}

IProperty* TextureDdsProperty::create(DOMNode* node) {
	DOMNamedNodeMap* attributes = node->getAttributes();
	DOMAttr* attr = (DOMAttr*)attributes->getNamedItem(XMLString::transcode("name"));
	const char* name = XMLString::transcode(attr->getValue());
	
	DOMNodeList* children = node->getChildNodes();
	unsigned int size = (unsigned int)children->getLength();
	std::string value;

	for(unsigned int i = 0; i < size; i++) {
		DOMNode* child = children->item(i);
		if(child->getNodeType() != DOMNode::ELEMENT_NODE)
			continue;
		char* value_name = XMLString::transcode(child->getNodeName());
		if(strcmp(value_name, "value") == 0) {
			value = XMLString::transcode(child->getTextContent());
			break;
		}
	}

	IProperty* ret;
	if(value == "")
		ret =  NULL;
	else
		ret = new TextureDdsProperty(value.c_str(), name);
	return ret;
}
