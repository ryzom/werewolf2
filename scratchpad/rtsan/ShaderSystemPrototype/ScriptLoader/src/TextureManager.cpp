#include "ScriptLoaderGeneral/TextureManager.h"
#include "../../nv_shared/nv_dds.h"
#include <stdio.h>

TextureManager::~TextureManager() {
	TextureManager::textureMap::iterator iter = m_textures.begin();
	for(;iter != m_textures.end(); iter++) {
		glDeleteTextures(1, &(iter->second));
	}
	m_textures.clear();
}

GLuint TextureManager::loadTexture2D(const char* file) {
	TextureManager::textureMap::iterator iter = m_textures.find(file);
	if(iter != m_textures.end())
		return iter->second;
	GLuint ret;
	glGenTextures(1, &ret);
	glBindTexture(GL_TEXTURE_2D, ret);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	
	printf("\nLoading DDS 2D image %s\n", file);
	nv_dds::CDDSImage image;
    if (image.load(file))
        image.upload_texture2D();
	else {
		printf("Unable to load texture %s\n", file);
		glDeleteTextures(1, &ret);
		return 0;
	}
	printf("Done making texture %s\n", file);

	char* nfile = new char[strlen(file)];
	memcpy(nfile, file, strlen(file)+1);
	m_textures[nfile] = ret;
	return ret;
}

GLuint TextureManager::loadTextureCube(const char* file) {
	TextureManager::textureMap::iterator iter = m_textures.find(file);
	if(iter != m_textures.end())
		return iter->second;
	GLuint ret;
	glGenTextures(1, &ret);
    glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, ret);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	printf("\nLoading DDS Cube image %s\n", file);
	nv_dds::CDDSImage image;
	if (image.load(file)) {
	    image.upload_textureCubemap();
	} else {
		printf("Unable to load texture %s\n", file);
		glDeleteTextures(1, &ret);
		return 0;
	}
	printf("Done making texture %s\n", file);

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);	

	char* nfile = new char[strlen(file)];
	memcpy(nfile, file, strlen(file)+1);
	m_textures[nfile] = ret;
	return ret;
}