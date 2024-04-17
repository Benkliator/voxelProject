#pragma once

#include <vector>

typedef unsigned int GLenum;

// Loads shaderfile, takes shadertype and filepath, returns shader ID
unsigned int loadShader(GLenum type, const char* filePath);

// Loads texturefile, returns linked texture ID
unsigned int loadTexture(const char* filepath, unsigned format);

// Loads 6 textures and maps them to a cube, returns linked texture ID
unsigned int loadTextureCube(std::vector<const char*> filepaths,
                             unsigned format);
