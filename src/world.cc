#include "glad/glad.h"

#include "utility.h"
#include "world.h"

#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <optional>

World::World(unsigned size, unsigned os) 
: renderDistance{ size }, offset{ os } {
    textureMap = loadTexture("./res/textures/Blockmap2.png", GL_RGBA);
    shaderInit();
    projection = glm::perspective(glm::radians(45.0f), 1.8f, 0.1f, 1000.0f);

    std::cout << "Creating chunks..." << std::endl;

    for (unsigned x = offset; x < offset + renderDistance; x++) {
        for (unsigned z = offset; z < offset + renderDistance; z++) {
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

std::optional<std::reference_wrapper<Chunk>>
World::getChunk(unsigned x, unsigned y, unsigned z) {
    glm::uvec3 findPos{ x, y, z };
    for (int i = 0; i < visibleChunks.size(); i++) {
        if (visibleChunks[i].getPos() == findPos) {
            return std::make_optional(std::ref(visibleChunks[i]));
        }
    }
    return std::nullopt;
}

void World::reloadChunksAround(unsigned x, unsigned y, unsigned z) {
    bool deleted = false;
    for (size_t i = 0; i < visibleChunks.size(); i++) {
        if (visibleChunks[i].getPos().x > x + ((renderDistance) * 8)) {
            visibleChunks.erase(visibleChunks.begin() + i--);
            deleted = true;
        }
    }
    if (!deleted) {
        return;
    }

    for (unsigned z = offset; z < offset + renderDistance; z++) {
        visibleChunks.emplace_back((x / 16) - ((renderDistance - 1) / 2), z, this);
    }
    for (unsigned z = 0; z < renderDistance; z++) {
        visibleChunks[visibleChunks.size() - 1 - z].generateMesh();
    }
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
