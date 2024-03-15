#include "../lib/world.h"
#include <iostream>

World::World(unsigned int size)
{
    std::cout << "Creating chunks..." << std::endl;
    if (size == 0) return;
    visibleChunks.push_back(Chunk{glm::vec3(0.0, 0.0, 0.0)});
    if (size == 1) return;

    for (int x = 1; x < size; x++)
    {
        for (int z = 1; z < size; z++)
        {
            visibleChunks.push_back(Chunk{glm::vec3( x, 0,  z)});
            visibleChunks.push_back(Chunk{glm::vec3(-x, 0, -z)});
            visibleChunks.push_back(Chunk{glm::vec3(-x, 0,  z)});
            visibleChunks.push_back(Chunk{glm::vec3( x, 0, -z)});

            visibleChunks.push_back(Chunk{glm::vec3( x    , 0,  z - 1)});
            visibleChunks.push_back(Chunk{glm::vec3( x - 1, 0, -z    )});
            visibleChunks.push_back(Chunk{glm::vec3(-x    , 0,  z - 1)});
            visibleChunks.push_back(Chunk{glm::vec3( x - 1, 0,  z    )});
        }
    }
    std::cout << "Creating meshes..." << std::endl;
    for (auto chunk : visibleChunks)
    {
        //chunk.cornerCheck( visibleChunks );
        auto [indices, vertices] = chunk.generateMesh();
        renderer.addRendering(vertices, indices);
    }
    std::cout << "Initiating rendering..." << std::endl;
    renderer.renderInit();
    std::cout << "Done!" << std::endl;
}

World::~World()
{
    std::vector<Chunk>().swap(visibleChunks);
}

void World::draw(glm::mat4 view)
{
    renderer.render(view);
}

World::Chunk::Chunk(glm::vec3 offset) {
    // Chunk offset
    chunkPos = glm::vec3(offset.x * 16, 0 * 16, offset.z * 16);
    generateTerrain();
}

// NOTE: Might need to actually impelent this function for memory safety (lol)
World::Chunk::~Chunk() {
    std::vector<unsigned long>().swap(blockArray);
}

bool World::Chunk::findBlock(glm::vec3 blockPos) {
    for (auto block : blockArray) {
        glm::vec3 pos((float)(block &                 0b1111),
                      (float)(block & 0b11111111111100000000) / 256.0f,
                      (float)(block &             0b11110000) / 16.0f);
        if (pos == blockPos) {
            return true;
        }
    }
    return false;
}

glm::vec3 World::Chunk::returnPos() const {
    return chunkPos;
}

std::pair< std::vector<unsigned int>, std::vector<float> > World::Chunk::generateMesh() {
    // NOTE: This implementation is quite slow, but very consitent.
    // It will also be easy to modify whenever normals are to be added, however
    // it can 100% be cleaned up and become even better.
    std::vector<unsigned int> indexMesh;
    std::vector<float>        vertexMesh;
    for (unsigned int it = 0; it < blockArray.size(); it++) {
        unsigned int block = blockArray[it];
        glm::vec3 pos((float)(block &                 0b1111),
                      (float)(block & 0b11111111111100000000) / 256.0f,
                      (float)(block &             0b11110000) / 16.0f);

        Block blockType( (block & 0b111100000000000000000000) / 1048576.0f );

        std::vector<bool> boolVector{
            obsTop(it)  , obsBack(it),
            obsFront(it), obsLeft(it),
            obsRight(it), obsBottom(it)
        };

        if(!boolVector[0])
        {
            indexMesh.push_back(  vertexMesh.size() / 8);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 2);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            for (size_t i = 0; i < 32; i+=8)
            {
                vertexMesh.push_back(topVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(topVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(topVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(topVertices[i + 3] + (blockType.top & 240)/16);
                vertexMesh.push_back(topVertices[i + 4] + (blockType.top & 15));
                vertexMesh.push_back(bottomVertices[i + 5] + pos.x + chunkPos.x);
                vertexMesh.push_back(bottomVertices[i + 6] + pos.y + chunkPos.y);
                vertexMesh.push_back(bottomVertices[i + 7] + pos.z + chunkPos.z);
            }
        }

        if( (!boolVector[1]) && pos.z != 0)
        {
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            indexMesh.push_back(  vertexMesh.size() / 8);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 2);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            for (size_t i = 0; i < 32; i+=8)
            {
                vertexMesh.push_back(backVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(backVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(backVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(backVertices[i + 3] + (blockType.side & 240)/16);
                vertexMesh.push_back(backVertices[i + 4] + (blockType.side & 15) );
                vertexMesh.push_back(bottomVertices[i + 5] + pos.x + chunkPos.x);
                vertexMesh.push_back(bottomVertices[i + 6] + pos.y + chunkPos.y);
                vertexMesh.push_back(bottomVertices[i + 7] + pos.z + chunkPos.z);
            }
        }
        if( (!boolVector[2]) && pos.z != 15)
        {
            indexMesh.push_back(  vertexMesh.size() / 8);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 2);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            for (size_t i = 0; i < 32; i+=8)
            {
                vertexMesh.push_back(frontVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(frontVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(frontVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(frontVertices[i + 3] + (blockType.side & 240)/16);
                vertexMesh.push_back(frontVertices[i + 4] + (blockType.side & 15));
                vertexMesh.push_back(bottomVertices[i + 5] + pos.x + chunkPos.x);
                vertexMesh.push_back(bottomVertices[i + 6] + pos.y + chunkPos.y);
                vertexMesh.push_back(bottomVertices[i + 7] + pos.z + chunkPos.z);
            }
        }
        if((!boolVector[3]) && pos.x != 0)
        {
            indexMesh.push_back( vertexMesh.size() / 8);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 2);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            for (size_t i = 0; i < 32; i+=8)
            {
                vertexMesh.push_back(leftVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(leftVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(leftVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(leftVertices[i + 3] + (blockType.side & 240)/16);
                vertexMesh.push_back(leftVertices[i + 4] + (blockType.side & 15));
                vertexMesh.push_back(bottomVertices[i + 5] + pos.x + chunkPos.x);
                vertexMesh.push_back(bottomVertices[i + 6] + pos.y + chunkPos.y);
                vertexMesh.push_back(bottomVertices[i + 7] + pos.z + chunkPos.z);
            }
        }
        if( (!boolVector[4]) && pos.x != 15)
        {
            indexMesh.push_back(  vertexMesh.size() / 8);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 2);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            for (size_t i = 0; i < 32; i+=8)
            {
                vertexMesh.push_back(rightVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(rightVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(rightVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(rightVertices[i + 3] + (blockType.side & 240)/16);
                vertexMesh.push_back(rightVertices[i + 4] + (blockType.side & 15));
                vertexMesh.push_back(bottomVertices[i + 5] + pos.x + chunkPos.x);
                vertexMesh.push_back(bottomVertices[i + 6] + pos.y + chunkPos.y);
                vertexMesh.push_back(bottomVertices[i + 7] + pos.z + chunkPos.z);
            }
        }
        if( (!boolVector[5]) && pos.y != 0)
        {
            indexMesh.push_back(  vertexMesh.size() / 8);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 2);
            indexMesh.push_back( (vertexMesh.size() / 8) + 1);
            indexMesh.push_back( (vertexMesh.size() / 8) + 3);
            for (size_t i = 0; i < 32; i+=8)
            {
                vertexMesh.push_back(bottomVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(bottomVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(bottomVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(bottomVertices[i + 3] + (blockType.bottom & 240)/16);
                vertexMesh.push_back(bottomVertices[i + 4] + (blockType.bottom & 15));
                vertexMesh.push_back(bottomVertices[i + 5] + pos.x + chunkPos.x);
                vertexMesh.push_back(bottomVertices[i + 6] + pos.y + chunkPos.y);
                vertexMesh.push_back(bottomVertices[i + 7] + pos.z + chunkPos.z);
            }
        }
    }
    return std::make_pair(indexMesh, vertexMesh);
}

void World::Chunk::generateTerrain() {
    float freq = 0.76323343; // Frequency
    float amp = 0.75;        // Amplifier
    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            // Uses perlin function to calculate height for xz position
            float height = perlin(((x + chunkPos.x) * freq) / 16,
                                 ((z + chunkPos.z) * freq)/ 16) * amp + 1;
            for (int y = 0; y < height; y++) {
                unsigned long val = ( ( ( 0 | x)
                                    | z *                 0b10000)
                                    | y *             0b100000000);
                // Using the following line in an if or switch case
                // can let us dynamically decide block generates at
                // different (x, y, z) values.
                val = (val | 0 * 0b100000000000000000000);
                blockArray.push_back( val );
            }
        }
    }
}

// z
bool World::Chunk::obsBack(unsigned int thisBlock) {
    if (thisBlock == 0) {
        return false;
    }
    int temp = thisBlock - 1;
    while ((blockArray[temp]           & 0b11110000)
        >= ((blockArray[thisBlock] & 0b11110000) - 16)
        && temp != -1) {
        if ((blockArray[temp]         & 0b111100001111)
            == (blockArray[thisBlock] & 0b111100001111)) {
            return true;
        }
        temp--;
    }
    return false;
}

// z
bool World::Chunk::obsFront(unsigned int thisBlock) {
    if (blockArray[thisBlock] == blockArray.back()) {
        return false;
    }
    int temp = thisBlock + 1;
    while ((blockArray[temp]           & 0b11110000)
        <= ((blockArray[thisBlock] & 0b11110000) + 16)
        && blockArray[temp - 1] != blockArray.back()) {
        if ((blockArray[temp]         & 0b111100001111)
            == (blockArray[thisBlock] & 0b111100001111)) {
            return true;
        }
        temp++;
    }
    return false;
}

// x
bool World::Chunk::obsLeft(unsigned int thisBlock) {
    if (thisBlock == 0){
        return false;
    }
    int temp = thisBlock - 1;
    while ((blockArray[temp]           & 0b1111)
        >= ((blockArray[thisBlock] & 0b1111) - 1)
        && temp != - 1) {
        if ((blockArray[temp]         & 0b111111110000)
            == (blockArray[thisBlock] & 0b111111110000)) {
            return true;
        }
        temp--;
    }
    return false;

}

// x
bool World::Chunk::obsRight(unsigned int thisBlock) {
    if (blockArray[thisBlock] == blockArray.back()) {
        return false;
    }
    int temp = thisBlock + 1;
    while ((blockArray[temp]          & 0b1111)
        <=((blockArray[thisBlock] & 0b1111) + 1)
        && blockArray[temp - 1] != blockArray.back()) {
        if ((blockArray[temp]         & 0b111111110000)
            == (blockArray[thisBlock] & 0b111111110000)) {
            return true;
        }
        temp++;
    }
    return false;
}

// y
bool World::Chunk::obsTop(unsigned int thisBlock) {
    if (blockArray[thisBlock] == blockArray.back()) {
        return false;
    }
    return( ((blockArray[thisBlock + 1] & 0b111100000000))
    == ((blockArray[thisBlock] & 0b111100000000) + 256));
}

// y
bool World::Chunk::obsBottom(unsigned int thisBlock){
    if (thisBlock == 0) {
        return false;
    }
    return( ((blockArray[thisBlock - 1] & 0b111100000000))
    == ((blockArray[thisBlock] & 0b111100000000) - 256));
}
