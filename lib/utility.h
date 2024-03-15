#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// TODO: Swallow pride and pick a name that doesn't have my name in it.

// Loads shaderfile, takes shadertype and filepath, returns shader ID
unsigned int loadShader(GLenum type, const char* filePath);

// Loads texturefile, returns linked texture ID
unsigned int loadTexture(const char* filepath, 
                         unsigned int format);

// Loads 6 textures and maps them to a cube, returns linked texture ID
unsigned int loadTextureCube(std::vector<const char*> filepaths, 
                             unsigned int format);
