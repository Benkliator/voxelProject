#include "glad/glad.h"

#include "utility.h"
#include "world.h"

#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <optional>
#include <algorithm>

World::World(unsigned size, unsigned offset) 
: renderDistance{ size } {
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
    std::for_each(visibleChunks.begin(), visibleChunks.end(),
                  [](Chunk& chunk) {
                    chunk.generateMesh();
                  });

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

// IDEA: Run this function whenever there is extra time to do stuff,
// and pre-load chunks outside of render distance without actually rendering
// , this would probably result in smoother world traversal.
void World::reloadChunksAround(unsigned x, unsigned y, unsigned z) {
    unsigned chunkVal = 0;
    for (size_t i = 0; i < visibleChunks.size(); i++) {
        if (visibleChunks[i].getPos().x > x + ((renderDistance) * 8)) {
            visibleChunks.erase(visibleChunks.begin() + i--);
            chunkVal = 1;
        } else if (visibleChunks[i].getPos().z > z + ((renderDistance) * 8)) {
            visibleChunks.erase(visibleChunks.begin() + i--);
            chunkVal = 2;
        } else if (visibleChunks[i].getPos().x < x - ((renderDistance) * 8)) {
            visibleChunks.erase(visibleChunks.begin() + i--);
            chunkVal = 3;
        } else if (visibleChunks[i].getPos().z < z - ((renderDistance) * 8)) {
            visibleChunks.erase(visibleChunks.begin() + i--);
            chunkVal = 4;
        }

    }

    switch (chunkVal) {
        case 0:
            return;
        case 1: {// -x 
                    for (unsigned zi = (z / 16) - (renderDistance  / 2); zi < (z / 16) + (renderDistance / 2); zi++) {
                        unsigned xi = (x / 16) - ((renderDistance - 1) / 2);
                        if (!getChunk(xi, 0, zi)) {
                            visibleChunks.emplace_back(xi, zi, this);
                            loadQueue.push(&visibleChunks.back());
                        }
                    }
                } return;
        case 2: { // -z
                    for (unsigned xi = (x / 16) - renderDistance / 2; xi < (x / 16) + renderDistance / 2; xi++) {
                        unsigned zi = (z / 16) - ((renderDistance - 1) / 2);
                        if (!getChunk(xi, 0, zi)) {
                            visibleChunks.emplace_back(xi, zi, this);
                            loadQueue.push(&visibleChunks.back());
                        }
                    }
                } return;

        case 3: { // +x
                    for (unsigned zi = (z / 16) - renderDistance / 2; zi < (z / 16) + renderDistance / 2; zi++) {
                        unsigned xi = (x / 16) + ((renderDistance - 1) / 2);
                        if (!getChunk(xi, 0, zi)) {
                            visibleChunks.emplace_back(xi, zi, this);
                            loadQueue.push(&visibleChunks.back());
                        }
                    }
                } return;
        case 4: { // +x
                    for (unsigned xi = (x / 16) - renderDistance / 2; xi < (x / 16) + renderDistance / 2; xi++) {
                        unsigned zi = (z / 16) + ((renderDistance - 1) / 2);
                        if (!getChunk(xi, 0, zi)) {
                            visibleChunks.emplace_back(xi, zi, this);
                            loadQueue.push(&visibleChunks.back());
                        }
                    }
                } return;
    }
}

// TODO: reload adjacent chunk meshes.
void World::meshCatchup() {
    if (loadQueue.empty())
        return;
    if (loadQueue.front())
        loadQueue.front()->generateMesh();
    loadQueue.pop();
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
        if(chunk.hasLoaded())
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
