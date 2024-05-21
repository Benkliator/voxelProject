#include "glad/glad.h"

#include "utility.h"
#include "world.h"

#include <algorithm>
#include <functional>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <optional>

World::World(unsigned size, glm::vec3 center) {
    if (!(size % 2)) {
        renderDistance = size + 1;
    } else {
        renderDistance = size;
    }

    unsigned xChunk = (unsigned)center.x - ((unsigned)(center.x) % 16);
    unsigned zChunk = (unsigned)center.z - ((unsigned)(center.z) % 16);
    worldCenter = glm::uvec3(xChunk, 0, zChunk);

    textureMap = loadTexture("./res/textures/Blockmap.png", GL_RGBA);
    shaderInit();
    projection = glm::perspective(glm::radians(60.0f), 1.8f, 0.1f, 1000.0f);

    std::cout << "Creating chunks..." << std::endl;

    int x = 0;
    int z = 0;
    int end = renderDistance * renderDistance;
    // Generate chunks in a spiral around the world center.
    for (int i = 0; i < end; i++) {
        int xp = x + renderDistance / 2;
        int zp = z + renderDistance / 2;
        if (xp >= 0 && static_cast<unsigned>(xp) < renderDistance && zp >= 0 &&
            static_cast<unsigned>(zp) < renderDistance) {
            visibleChunks.emplace_back(
                x + (xChunk / 16), z + (zChunk / 16), this);
        }

        if (abs(x) <= abs(z) && (x != z || x >= 0)) {
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
    unsigned xChunk = x & ~chunkMask;
    unsigned zChunk = z & ~chunkMask;
    unsigned xBlockOffset = x & chunkMask;
    unsigned zBlockOffset = z & chunkMask;
    for (Chunk& chunk : visibleChunks) {
        glm::uvec3 chunkpos = chunk.getPos();
        if (chunkpos.x == xChunk && chunkpos.z == zChunk) {
            return chunk.getBlock(xBlockOffset, y, zBlockOffset);
        }
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<Chunk>>
World::getChunk(unsigned x, unsigned y, unsigned z) {
    glm::uvec3 findPos{ x, y, z };
    for (size_t i = 0; i < visibleChunks.size(); i++) {
        if (visibleChunks[i].getPos() == findPos) {
            return std::make_optional(std::ref(visibleChunks[i]));
        }
    }
    return std::nullopt;
}

void World::reloadChunksAround(unsigned xChunk,
                               unsigned yChunk,
                               unsigned zChunk) {
    worldCenter = glm::uvec3(xChunk, yChunk, zChunk);
    size_t size = visibleChunks.size();
    for (size_t i = 0; i < size; i++) {
        if (visibleChunks[i].distanceFrom(worldCenter) >
            (renderDistance * 8) /* RD / 2 * 16 */) {
            if (visibleChunks[i].getPos().x >
                xChunk + ((renderDistance) * 8)) { // -x
                Chunk chunk{ (visibleChunks[i].getPos().x / 16) -
                                 renderDistance,
                             (visibleChunks[i].getPos().z / 16),
                             this };
                visibleChunks[i] = chunk;
                chunk.findAdjacentChunks();
                if (chunk.getRightChunk()) {
                    loadQueue.push(chunk.getRightChunk());
                }
                loadQueue.push(&visibleChunks[i]);
                continue;
            } else if (visibleChunks[i].getPos().z >
                       zChunk + ((renderDistance) * 8)) { // -z
                Chunk chunk{ (visibleChunks[i].getPos().x / 16),
                             (visibleChunks[i].getPos().z / 16) -
                                 renderDistance,
                             this };
                visibleChunks[i] = chunk;
                chunk.findAdjacentChunks();
                if (chunk.getFrontChunk()) {
                    loadQueue.push(chunk.getFrontChunk());
                }
                loadQueue.push(&visibleChunks[i]);
                continue;
            } else if (visibleChunks[i].getPos().x <
                       xChunk - ((renderDistance) * 8)) { // +x
                Chunk chunk{ (visibleChunks[i].getPos().x / 16) +
                                 renderDistance,
                             (visibleChunks[i].getPos().z / 16),
                             this };
                visibleChunks[i] = chunk;
                chunk.findAdjacentChunks();
                if (chunk.getLeftChunk()) {
                    loadQueue.push(chunk.getLeftChunk());
                }
                loadQueue.push(&visibleChunks[i]);
                continue;
            } else if (visibleChunks[i].getPos().z <
                       zChunk - ((renderDistance) * 8)) { // +z
                Chunk chunk{ (visibleChunks[i].getPos().x / 16u),
                             (visibleChunks[i].getPos().z / 16u) +
                                 renderDistance,
                             this };
                visibleChunks[i] = chunk;
                chunk.findAdjacentChunks();
                if (chunk.getBackChunk()) {
                    loadQueue.push(chunk.getBackChunk());
                }
                loadQueue.push(&visibleChunks[i]);
                continue;
            }
        }
    }
    /*
     * Sort the remaining elements with the first element centered,
     * continue the spiral generation from the end. 
     * (Outdated comment but leave it for now)
     */
}

void World::meshCatchup() {
    if (!loadQueue.front()) {
        while (!loadQueue.front() && !loadQueue.empty()) {
            loadQueue.pop();
        }
    }
    if (!loadQueue.empty()) {
        loadQueue.front()->clearMesh();
        loadQueue.front()->generateMesh();
        loadQueue.pop();
    }
}

// in fact not yet threadsafe B)
void World::threadsafeMeshCatchup() {
    using namespace std::chrono_literals;
    while (meshLoad) {
        while (!loadQueue.empty()) {
            if (loadQueue.front()) {
                Chunk* chunk = loadQueue.front();
                chunk->clearMesh();
                chunk->generateMesh();
            }
            loadQueue.pop();
        }

        const auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(500ms);
        const auto end = std::chrono::high_resolution_clock::now();
        const std::chrono::duration<double, std::milli> elapsed = end - start;
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
        if (chunk.hasLoaded())
            chunk.draw(shaderProgram);
    }
}

void World::shaderInit() {
    unsigned vertexShader =
        loadShader(GL_VERTEX_SHADER, "./res/shaders/world.vert");
    unsigned fragmentShader =
        loadShader(GL_FRAGMENT_SHADER, "./res/shaders/world.frag");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
