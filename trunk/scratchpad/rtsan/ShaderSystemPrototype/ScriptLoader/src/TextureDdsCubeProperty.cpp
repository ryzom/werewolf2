#include "PropertyManager/TextureDdsCubeProperty.h"
#include "ScriptLoader/ScriptVariable.h"
#include "ScriptLoaderGeneral/TextureManager.h"

XERCES_CPP_NAMESPACE_USE

TextureDdsCubeProperty::TextureDdsCubeProperty(const char* file, const char* name) {
	setName(name);

	m_file = file;

	m_value = TextureManager::instance().loadTextureCube(file);
}

TextureDdsCubeProperty::~TextureDdsCubeProperty() {
	glDeleteTextures(1, &m_value);
}

void TextureDdsCubeProperty::setCGGLParameter(CGparameter& parameter) {
	cgGLSetTextureParameter(parameter, m_value);
}

void TextureDdsCubeProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME
}

void TextureDdsCubeProperty::getHash(std::stringstream& hash) {
	hash << m_file;
}

IProperty* TextureDdsCubeProperty::create(DOMNode* node) {
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
		ret = new TextureDdsCubeProperty(value.c_str(), name);
	return ret;
}
