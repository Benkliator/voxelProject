#pragma once

#include "glad/glad.h"

#include <vector>

// Loads shaderfile, takes shadertype and filepath, returns shader ID
unsigned loadShader(GLenum type, const char* filePath);

// Loads texturefile, returns linked texture ID
unsigned loadTexture(const char* filepath, unsigned format);

// Loads 6 textures and maps them to a cube, returns linked texture ID
unsigned loadTextureCube(std::vector<const char*> filepaths, unsigned format);
