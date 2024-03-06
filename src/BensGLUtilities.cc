#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include"../lib/BensGLUtilities.h"


// Loads shaderfile, takes shadertype and filepath, returns shader
unsigned int loadShader(GLenum type, const char* filePath)
{
    // Read shader code from file
    std::ifstream shaderFile(filePath);
    if (!shaderFile.is_open()) 
    {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return 0;
    }

    // Load shader code into string
    std::string shaderCode((std::istreambuf_iterator<char>(shaderFile)), std::istreambuf_iterator<char>());
    shaderFile.close();

    // Compile shader 
    GLuint shader = glCreateShader(type);

    const char* shaderCodePtr = shaderCode.c_str();
    glShaderSource(shader, 1, &shaderCodePtr, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader compilation error in " << filePath << ":\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}


unsigned int loadTexture( const char* filepath, 
                          unsigned int format)
{
    unsigned int texture;
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filepath, &width, &height, &nrChannels, 0);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load and generate the texture
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "bad texture" << std::endl;
    }
    stbi_image_free(data);

    return texture;
}

unsigned int  loadTextureCube(std::vector<const char*> filepath, 
                              unsigned int format)
{
    unsigned int texture;
    int width, height, nrChannels;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char *data = stbi_load(filepath[i], &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            std::cout << "bad texture" << std::endl;
        }
        stbi_image_free(data);
    }

    return texture; 
}
