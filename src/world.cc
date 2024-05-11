#include "glad/glad.h"

#include "utility.h"
#include "world.h"

#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <optional>
#include <algorithm>

World::World(unsigned size, unsigned offset, glm::vec3 center) {
    if (!(size % 2)) {
        renderDistance = size + 1;
    } else {
        renderDistance = size;
    }

    unsigned xChunk = (unsigned)center.x - ((unsigned)(center.x) % 16);
    unsigned zChunk = (unsigned)center.z - ((unsigned)(center.z) % 16);
    worldCenter = glm::uvec3(xChunk, 0, zChunk);
    
    textureMap = loadTexture("./res/textures/Blockmap2.png", GL_RGBA);
    shaderInit();
    projection = glm::perspective(glm::radians(60.0f), 1.8f, 0.1f, 1000.0f);

    std::cout << "Creating chunks..." << std::endl;

    int x = 0;
    int z = 0;
    int end = renderDistance * renderDistance;
    // Generate chunks in a spiral around the world center.
    for(int i = 0; i < end; i++) {
        int xp = x + renderDistance / 2;
        int zp = z + renderDistance / 2;
        if(xp >= 0 && xp < renderDistance && zp >= 0 && zp < renderDistance) {
            visibleChunks.emplace_back(x + (xChunk / 16), z + (zChunk / 16), this);
        }

        if(abs(x) <= abs(z) && (x != z || x >= 0)) {
            x += ((z >= 0) ? 1 : -1);
        } else {
            z += ((x >= 0) ? -1 : 1);
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
void World::reloadChunksAround(unsigned xChunk, unsigned yChunk, unsigned zChunk) {
    worldCenter = glm::uvec3(xChunk, yChunk, zChunk);
    unsigned size = visibleChunks.size();
    for (int i = 0; i < size; i++) {
        if (visibleChunks[i].distanceFrom(worldCenter) > (renderDistance * 8) /* RD / 2 * 16 */) {
            if (visibleChunks[i].getPos().x > xChunk + ((renderDistance) * 8)) {        // -x
                Chunk chunk{(visibleChunks[i].getPos().x / 16) - renderDistance,
                            (visibleChunks[i].getPos().z / 16),
                             this};
                visibleChunks[i] = chunk;
                auto chunkOpt = getChunk(chunk.getPos().x + 16, 
                                         0,
                                         chunk.getPos().z);
                if (chunkOpt.has_value()) {
                    loadQueue.push(&chunkOpt.value().get());
                }
                loadQueue.push(&visibleChunks[i]);
                continue;
            } else if (visibleChunks[i].getPos().z > zChunk + ((renderDistance) * 8)) { // -z
                Chunk chunk{(visibleChunks[i].getPos().x / 16),
                            (visibleChunks[i].getPos().z / 16) - renderDistance,
                             this};
                visibleChunks[i] = chunk;
                auto chunkOpt = getChunk(chunk.getPos().x, 0,
                                         chunk.getPos().z + 16);
                if (chunkOpt.has_value()) {
                    loadQueue.push(&chunkOpt.value().get());
                }
                loadQueue.push(&visibleChunks[i]);
                continue;
            } else if (visibleChunks[i].getPos().x < xChunk - ((renderDistance) * 8)) { // +x
                Chunk chunk{(visibleChunks[i].getPos().x / 16) + renderDistance,
                            (visibleChunks[i].getPos().z / 16),
                             this};
                visibleChunks[i] = chunk;
                auto chunkOpt = getChunk(chunk.getPos().x - 16,
                                         0,
                                         chunk.getPos().z);
                if (chunkOpt.has_value()) {
                    loadQueue.push(&chunkOpt.value().get());
                }
                loadQueue.push(&visibleChunks[i]);
                continue;
            } else if (visibleChunks[i].getPos().z < zChunk - ((renderDistance) * 8)) { // +z
                Chunk chunk{(visibleChunks[i].getPos().x / 16u),
                            (visibleChunks[i].getPos().z / 16u) + renderDistance,
                             this};
                visibleChunks[i] = chunk;
                auto chunkOpt = getChunk(chunk.getPos().x,
                                         0,
                                         chunk.getPos().z - 16);
                if (chunkOpt.has_value()) {
                    loadQueue.push(&chunkOpt.value().get());
                }
                loadQueue.push(&visibleChunks[i]);
                continue;
            }
        }
    }
    /*
     * Sort the remaining elements to the first element is centered, 
     * continue the spiral generation from the end. (Outdated comment but leave it for now)
     */ 
}

// TODO: reload adjacent chunk meshes.
void World::meshCatchup() {
    if (!loadQueue.empty() && loadQueue.front()) {
        loadQueue.front()->generateMesh();
        loadQueue.pop();
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
