#include "world.h"

World::World(unsigned int size) {
    textureMap = loadTexture("./res/textures/Blockmap2.png", GL_RGBA);
    shaderInit();
    projection = glm::perspective(glm::radians(45.0f), 1.8f, 0.1f, 1000.0f);

    std::cout << "Creating chunks..." << std::endl;

    for (int x = 0; x < size; x++) {
        std::vector<Chunk> temp;
        for (int z = 0; z < size; z++) {
            visibleChunks.emplace_back(x, 0, z);
        }
    }

    std::cout << "Creating meshes..." << std::endl;

    for (auto& chunk : visibleChunks) {
        chunk.generateMesh();
    }
    std::cout << "Done!" << std::endl;
    std::cout << "Created " << visibleChunks.size() << " chunks!" << std::endl;
}

World::~World() {
    // std::vector<Chunk>().swap(visibleChunks);
}

void World::draw(glm::mat4& view) {
    glUseProgram(shaderProgram);
    glBindTexture(GL_TEXTURE_2D, textureMap);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                       1,
                       GL_FALSE,
                       &projection[0][0]);
    glUniformMatrix4fv(
        glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, &view[0][0]);

    for (auto& chunk : visibleChunks) {
        chunk.draw();
    }
}

void World::shaderInit() {
    unsigned int vertexShader =
        loadShader(GL_VERTEX_SHADER, "./res/shaders/main.vert");
    unsigned int fragmentShader =
        loadShader(GL_FRAGMENT_SHADER, "./res/shaders/main.frag");

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

World::Chunk::Chunk(int x, int y, int z) {
    // Chunk offset
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    pos.x = x * 16;
    pos.y = y * 16;
    pos.z = z * 16;
    generateTerrain();
}

// NOTE: Might need to actually impelent this function for memory safety (lol)
World::Chunk::~Chunk() {
    // std::vector<unsigned int>().swap(blockArray);
}

void World::Chunk::generateMesh() {
    // NOTE: This implementation is quite slow, but very consitent.
    // it can 100% be cleaned up and become even better
    std::vector<float> vertexMesh;
    std::vector<unsigned int> indexMesh;
    for (unsigned int it = 0; it < blockArray.size(); it++) {
        unsigned int block = blockArray[it];
        if ((block & blockTypeBits) == Block::Air) {
            continue;
        }

        unsigned int x = (block & xBits) >> xBitOffset;
        unsigned int y = (block & yBits) >> yBitOffset;
        unsigned int z = (block & zBits) >> zBitOffset;
        Block blockType((block & blockTypeBits) >> blockTypeBitOffset);

        // (!obsTop(it))
        if (y < worldHeight &&
            ((getBlock(x, y + 1, z) & blockTypeBits) == Block::Air)) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            std::array<float, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Top);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(topVertices[i] + x + pos.x);
                vertexMesh.push_back(topVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(topVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    topVertices[i + 3] +
                    static_cast<float>((blockType.top & zBits) >> zBitOffset));
                vertexMesh.push_back(topVertices[i + 4] +
                                     (blockType.top & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(occlusionArray[i / 8]);
            }
        }

        // (!obsBack(it) && z != 0)
        if (z > 0 && ((getBlock(x, y, z - 1) & blockTypeBits) == Block::Air)) {
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            std::array<float, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Back);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(backVertices[i] + x + pos.x);
                vertexMesh.push_back(backVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(backVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    backVertices[i + 3] +
                    static_cast<float>((blockType.side & zBits) >> zBitOffset));
                vertexMesh.push_back(backVertices[i + 4] +
                                     (blockType.side & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(occlusionArray[i / 8]);
            }
        }

        // (!obsFront(it) && z != 15)
        if (z < 15 && ((getBlock(x, y, z + 1) & blockTypeBits) == Block::Air)) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            std::array<float, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Front);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(frontVertices[i] + x + pos.x);
                vertexMesh.push_back(frontVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(frontVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    frontVertices[i + 3] +
                    static_cast<float>((blockType.side & zBits) >> zBitOffset));
                vertexMesh.push_back(frontVertices[i + 4] +
                                     (blockType.side & 15));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(occlusionArray[i / 8]);
            }
        }

        // (!obsLeft(it) && x != 0)
        if (x > 0 && ((getBlock(x - 1, y, z) & blockTypeBits) == Block::Air)) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            std::array<float, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Left);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(leftVertices[i] + x + pos.x);
                vertexMesh.push_back(leftVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(leftVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    leftVertices[i + 3] +
                    static_cast<float>((blockType.side & zBits) >> zBitOffset));
                vertexMesh.push_back(leftVertices[i + 4] +
                                     (blockType.side & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(occlusionArray[i / 8]);
            }
        }

        // (!obsRight(it) && x != 15)
        if (x < 15 && ((getBlock(x + 1, y, z) & blockTypeBits) == Block::Air)) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            std::array<float, 4> occlusionArray =
                getOcclusion(x, y, z, Block::Right);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(rightVertices[i] + x + pos.x);
                vertexMesh.push_back(rightVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(rightVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    rightVertices[i + 3] +
                    static_cast<float>((blockType.side & zBits) >> zBitOffset));
                vertexMesh.push_back(rightVertices[i + 4] +
                                     (blockType.side & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(occlusionArray[i / 8]);
            }
        }

        // (!obsBottom(it) && y != 0)
        if (y > 0 && ((getBlock(x, y - 1, z) & blockTypeBits) == Block::Air)) {
            indexMesh.push_back(vertexMesh.size() / 9);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 2);
            indexMesh.push_back((vertexMesh.size() / 9) + 1);
            indexMesh.push_back((vertexMesh.size() / 9) + 3);
            for (size_t i = 0; i < 32; i += 8) {
                vertexMesh.push_back(bottomVertices[i] + x + pos.x);
                vertexMesh.push_back(bottomVertices[i + 1] + y + pos.y);
                vertexMesh.push_back(bottomVertices[i + 2] + z + pos.z);
                vertexMesh.push_back(
                    bottomVertices[i + 3] +
                    static_cast<float>((blockType.bottom & zBits) >>
                                       zBitOffset));
                vertexMesh.push_back(bottomVertices[i + 4] +
                                     (blockType.bottom & xBits));
                vertexMesh.push_back(bottomVertices[i + 5]);
                vertexMesh.push_back(bottomVertices[i + 6]);
                vertexMesh.push_back(bottomVertices[i + 7]);
                vertexMesh.push_back(3);
            }
        }
    }
    renderInit(vertexMesh, indexMesh);
}

void World::Chunk::draw() {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
}

// TODO(Christoffer): Should create a World::getBlock and use with the chunk's
//                    world pointer instead
unsigned int
World::Chunk::getBlock(unsigned int x, unsigned int y, unsigned int z) {
    size_t ix = y + (z * worldHeight) + (x * 16 * worldHeight);
    return blockArray[ix];
}

chunkPos World::Chunk::getPos() {
    return pos;
}

void World::Chunk::generateTerrain() {
    float freq = 0.76323343; // Frequency
    float amp = 0.75;        // Amplifier
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            // Uses perlin function to calculate height for xz position
            float noise =
                perlin(((x + pos.x) * freq) / 16, ((z + pos.z) * freq) / 16);
            float height = noise * amp + 1;
            for (int y = 0; y < worldHeight; y++) {
                // Using the following line in an if or switch case
                // can let you dynamically decide block generations at
                // different (x, y, z) values.
                enum Block::BlockType bt;
                if (y < height) {
                    bt = Block::Grass;
                } else {
                    bt = Block::Air;
                }
                unsigned int val = x << xBitOffset | y << yBitOffset |
                                   z << zBitOffset | bt << blockTypeBitOffset;
                blockArray.push_back(val);
            }
        }
    }
}

void World::Chunk::renderInit(std::vector<float> vertexMesh, std::vector<unsigned int>indexMesh) {
    indexSize = indexMesh.size();
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexMesh.size() * sizeof(float),
                 vertexMesh.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indexMesh.size() * sizeof(unsigned int),
                 indexMesh.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3,
                          1,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
}


std::array<float, 4> World::Chunk::getOcclusion(unsigned int x, unsigned int y, unsigned int z, unsigned short int face) {
    std::array<float, 4> vertexOcclusion{0, 0, 0, 0};
    switch (face) {
        case Block::Top: {
            if ((getBlock(x, y + 1, z + 1) & blockTypeBits) != Block::Air) {
                vertexOcclusion[1] += 1;
                vertexOcclusion[2] += 1;
            } if ((z > 0) && (getBlock(x, y + 1, z - 1) & blockTypeBits) != Block::Air) {
                vertexOcclusion[0] += 1;
                vertexOcclusion[3] += 1;
            } if ((getBlock(x + 1, y + 1, z) & blockTypeBits) != Block::Air) {
                vertexOcclusion[0] += 1;
                vertexOcclusion[1] += 1;
            } if ((x > 0) && (getBlock(x - 1, y + 1, z) & blockTypeBits) != Block::Air) {
                vertexOcclusion[2] += 1;
                vertexOcclusion[3] += 1;
            }

            if ((getBlock(x + 1, y + 1, z + 1) & blockTypeBits) != Block::Air) {
                vertexOcclusion[1] += 1;
            } if ((x > 0) && (getBlock(x - 1, y + 1, z + 1) & blockTypeBits) != Block::Air) {
                vertexOcclusion[2] += 1;
            } if ((z > 0) && (getBlock(x + 1, y + 1, z - 1) & blockTypeBits) != Block::Air) {
                vertexOcclusion[0] += 1;
            } if ((x > 0) && (z > 0) && (getBlock(x - 1, y + 1, z - 1) & blockTypeBits) != Block::Air) {
                vertexOcclusion[3] += 1;
            }
        } break;
        case Block::Back: {
            if (y == 0)
                break;
            if ((z > 0) && (getBlock(x, y - 1, z - 1) & blockTypeBits) != Block::Air) {
                vertexOcclusion[1] = 3;
                vertexOcclusion[2] = 3;
            }
        } break;
        case Block::Front: {
            if (y == 0)
                break;
            if ((getBlock(x, y - 1, z + 1) & blockTypeBits) != Block::Air) {
                vertexOcclusion[1] = 3;
                vertexOcclusion[2] = 3;
            }
        } break;
        case Block::Right: {
            if (y == 0)
                break;
            if ((getBlock(x + 1, y - 1, z) & blockTypeBits) != Block::Air) {
                vertexOcclusion[1] = 3;
                vertexOcclusion[2] = 3;
            }
        } break;
        case Block::Left: {
            if (y == 0)
                break;
            if ((getBlock(x - 1, y - 1, z) & blockTypeBits) != Block::Air) {
                vertexOcclusion[2] = 3;
                vertexOcclusion[1] = 3;
            }
        } break;
    }
    return vertexOcclusion;
}

