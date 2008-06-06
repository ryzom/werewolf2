#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__

#include <hash_map>
#include "ISingleton.h"
#include "general.h"
#include <Cg/cgGL.h>

#include "ScriptLoaderDefs.h"

class SCRIPT_LOADER_API TextureManager : public ISingleton<TextureManager> {
public:
	~TextureManager();

	GLuint loadTexture2D(const char* file);
	GLuint loadTextureCube(const char* file);

private:
	typedef std::hash_map<const char*, GLuint, std::hash<const char*>, streqpred> textureMap;
	textureMap m_textures;
};

#endif __TEXTUREMANAGER_H__