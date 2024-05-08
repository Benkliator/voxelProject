#include "glad/glad.h"

#include "utility.h"
#include "world.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

World::World(unsigned size) {
    textureMap = loadTexture("./res/textures/Blockmap2.png", GL_RGBA);
    shaderInit();
    projection = glm::perspective(glm::radians(45.0f), 1.8f, 0.1f, 1000.0f);

    std::cout << "Creating chunks..." << std::endl;

    for (unsigned x = 0; x < size; x++) {
        for (unsigned z = 0; z < size; z++) {
            visibleChunks.emplace_back(x, z, this);
        }
    }

    std::cout << "Creating meshes..." << std::endl;

    for (Chunk& chunk : visibleChunks) {
        chunk.generateMesh();
    }

    std::cout << "Done!" << std::endl;
    std::cout << "Created " << visibleChunks.size() << " chunks!" << std::endl;
}

std::optional<ushort> World::getBlock(long x, long y, long z) {
    const unsigned chunkMask = 0b1111;
    unsigned chunkX = x & ~chunkMask;
    unsigned chunkZ = z & ~chunkMask;
    unsigned blockOffsetX = x & chunkMask;
    unsigned blockOffsetZ = z & chunkMask;
    for (Chunk& chunk : visibleChunks) {
        glm::uvec3 chunkpos = chunk.getPos();
        if (chunkpos.x == chunkX && chunkpos.z == chunkZ) {
            return chunk.getBlock(blockOffsetX, y, blockOffsetZ);
        }
    }
    return std::nullopt;
}

Chunk* World::getChunk(unsigned x, unsigned y, unsigned z) {
    glm::uvec3 findPos{ x, y, z };
    for (Chunk& chunk : visibleChunks) {
        if (chunk.getPos() == findPos) {
            return &chunk;
        }
    }
    return nullptr;
}

World::~World() {}

void World::draw(glm::mat4& view) {
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, textureMap);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(projection));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
                       1,
                       GL_FALSE,
                       glm::value_ptr(view));

    for (Chunk& chunk : visibleChunks) {
        chunk.draw(shaderProgram);
    }
}

void World::shaderInit() {
    unsigned vertexShader =
        loadShader(GL_VERTEX_SHADER, "./res/shaders/main.vert");
    unsigned fragmentShader =
        loadShader(GL_FRAGMENT_SHADER, "./res/shaders/main.frag");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
