#include "glad/glad.h"

#include "game.h"
#include "hud.h"
#include "utility.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Hud::Hud() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // disable byte-alignment restriction for drawing text
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library"
                  << std::endl;
    }

    if (FT_New_Face(ft, "res/fonts/MinecraftTen.ttf", 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    unsigned textures[NCHARS];
    glGenTextures(NCHARS, textures);
    for (unsigned char charIx = 0; charIx < NCHARS; charIx++) {
        // load character glyph
        if (FT_Load_Char(face, charIx, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        glBindTexture(GL_TEXTURE_2D, textures[charIx]);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);
        // texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        characters.emplace(
            charIx,
            Character{
                textures[charIx],
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                face->glyph->advance.x,
            });
    }
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    unsigned vertexShader =
        loadShader(GL_VERTEX_SHADER, "./res/shaders/hud.vert");
    unsigned fragmentShader =
        loadShader(GL_FRAGMENT_SHADER, "./res/shaders/hud.frag");
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glm::mat4 projection = glm::ortho(0.0f,
                                      static_cast<float>(SCR_WIDTH),
                                      0.0f,
                                      static_cast<float>(SCR_HEIGHT));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(projection));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
}

void Hud::renderText(std::string text, float x, float y, float scale, glm::vec3 color) {
    // activate corresponding render state
    glUseProgram(shaderProgram);
    glUniform3f(glGetUniformLocation(shaderProgram, "textColor"),
                color.x,
                color.y,
                color.z);
    // glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
        Character& ch = characters.at(*c);

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;
        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float vertices[6][4] = {
            { xpos, ypos + h, 0.0f, 0.0f }, { xpos, ypos, 0.0f, 1.0f },
            { xpos + w, ypos, 1.0f, 1.0f }, { xpos, ypos + h, 0.0f, 0.0f },
            { xpos + w, ypos, 1.0f, 1.0f }, { xpos + w, ypos + h, 1.0f, 0.0f },
        };
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // bitshift by 6 to get value in pixels (2^6 = 64)
        x += (ch.advance >> 6) * scale;
    }
}
