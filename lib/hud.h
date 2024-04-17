#pragma once
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_int2.hpp>

#include <ft2build.h>
#include <map>

#include <string>
#include FT_FREETYPE_H

#define NCHARS 128

class Hud {
public:
    Hud();
    void renderText(std::string, float, float, float scale, glm::vec3 color);

private:
    FT_Library ft;
    FT_Face face;

    struct Character {
        // ID handle of the glyph texture
        unsigned textureID;
        // Size of glyph
        glm::ivec2 size;
        // Offset from baseline to left/top of glyph
        glm::ivec2 bearing;
        // Offset to advance to next glyph
        long advance;
    };
    std::map<char, Character> characters;

    unsigned VAO;
    unsigned VBO;
    unsigned shaderProgram;
};
