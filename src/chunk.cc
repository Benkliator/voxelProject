#include "../lib/chunk.h"
#include "../lib/BensGLUtilities.h"

Chunk::Chunk(glm::vec3 offset) {
    // Chunk offset
    chunkPos = glm::vec3(offset.x * 16, 0 * 16, offset.z * 16);
    generateTerrain();
}

// NOTE: Might need to actually impelent this function for memory safety (lol)
Chunk::~Chunk() {
    std::vector<unsigned long>().swap(blockArray);
}


// NOTE: This function works but is the absolute
// worst function in the entire system, it currently just
// exists as a lazy proof of concept. Instead, store adjacent chunks
// efficiently in the world class, and please dont make n^3 functions
// like these.
//
// NOTE of NOTE: I have made the function better by removing its contents.
void Chunk::cornerCheck(std::vector<Chunk>& otherChunk){}

bool Chunk::findBlock(glm::vec3 blockPos) {
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

glm::vec3 Chunk::returnPos() {
    return chunkPos;
}

void Chunk::generateTerrain() {
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


std::pair< std::vector<unsigned int>, std::vector<float> > Chunk::generateMesh() {
    std::vector<unsigned int> indexMesh;
    std::vector<float>        vertexMesh;
   // NOTE: This implementation is quite slow, but very consitent.
   // It will also be easy to modify whenever normals are to be added, however
   // it can 100% be cleaned up and become even better.
   for (unsigned int it = 0; it < blockArray.size(); it++) {
        unsigned int block = blockArray[it];
        glm::vec3 pos((float)(block &                 0b1111),
                      (float)(block & 0b11111111111100000000) / 256.0f,
                      (float)(block &             0b11110000) / 16.0f);

        Block blockType( (block & 0b111100000000000000000000) / 1048576.0f );
        std::vector<bool>   boolVector{obsTop(it), obsBack(it), 
                                       obsFront(it), obsLeft(it), 
                                       obsRight(it), obsBottom(it)};
        unsigned int count = 0;
        for(int i = 0; i < boolVector.size(); i++) {
            if (!boolVector[i]) {
                count = pow(2, i);
            }
        }
        if (!count) {
            continue;
        }
        if (count == 1) {
            indexMesh.push_back( 0 );
            indexMesh.push_back( 3 );
            indexMesh.push_back( 1 );
            indexMesh.push_back( 2 );
            indexMesh.push_back( 1 );
            indexMesh.push_back( 3 );

            vertexMesh.push_back(blockVertices[20] + pos.x + chunkPos.x);
            vertexMesh.push_back(blockVertices[21] + pos.y);
            vertexMesh.push_back(blockVertices[22] + pos.z + chunkPos.z);
            vertexMesh.push_back(blockVertices[23] + (blockType.top & 240)/16);
            vertexMesh.push_back(blockVertices[24] + (blockType.top & 15));

            vertexMesh.push_back(blockVertices[0] + pos.x + chunkPos.x);
            vertexMesh.push_back(blockVertices[1] + pos.y);
            vertexMesh.push_back(blockVertices[2] + pos.z + chunkPos.z);
            vertexMesh.push_back(blockVertices[3] + (blockType.top & 240)/16);
            vertexMesh.push_back(blockVertices[4] + (blockType.top & 15));

            vertexMesh.push_back(blockVertices[15] + pos.x + chunkPos.x);
            vertexMesh.push_back(blockVertices[16] + pos.y);
            vertexMesh.push_back(blockVertices[17] + pos.z + chunkPos.z);
            vertexMesh.push_back(blockVertices[18] + (blockType.top & 240)/16);
            vertexMesh.push_back(blockVertices[19] + (blockType.top & 15));

            vertexMesh.push_back(blockVertices[35] + pos.x + chunkPos.x);
            vertexMesh.push_back(blockVertices[36] + pos.y);
            vertexMesh.push_back(blockVertices[37] + pos.z + chunkPos.z);
            vertexMesh.push_back(blockVertices[38] + (blockType.top & 240)/16);
            vertexMesh.push_back(blockVertices[39] + (blockType.top & 15));
            continue;
        }
        continue;

        for (int i = 0; i < 40; i+=5) {
            vertexMesh.push_back(blockVertices[i + 0] + pos.x);
            vertexMesh.push_back(blockVertices[i + 1] + pos.y);
            vertexMesh.push_back(blockVertices[i + 2] + pos.z);
            vertexMesh.push_back(blockVertices[i + 3] + (blockType.side & 240)/16);
            vertexMesh.push_back(blockVertices[i + 4] + (blockType.side & 15));
        }

        switch (count) {
            case 0b000010: // Only back side
                if (pos.z != 0) {
                indexMesh.push_back( 7 );
                indexMesh.push_back( 4 );
                indexMesh.push_back( 5 );
                indexMesh.push_back( 6 );
                indexMesh.push_back( 7 );
                indexMesh.push_back( 5 );
                }
                continue;
            case 0b000100: // Only front side
                if (pos.z != 15) {
                indexMesh.push_back( 0 );
                indexMesh.push_back( 3 );
                indexMesh.push_back( 1 );
                indexMesh.push_back( 2 );
                indexMesh.push_back( 1 );
                indexMesh.push_back( 3 );
                }
                continue;
            case 0b001000: // Only left side
                if (pos.x != 0) {
                indexMesh.push_back( 3 );
                indexMesh.push_back( 7 );
                indexMesh.push_back( 2 );
                indexMesh.push_back( 6 );
                indexMesh.push_back( 2 );
                indexMesh.push_back( 7 );
                }
                continue;
            case 0b010000: // Only right side
                if (pos.x != 15) {
                indexMesh.push_back( 4 );
                indexMesh.push_back( 0 );
                indexMesh.push_back( 5 );
                indexMesh.push_back( 1 );
                indexMesh.push_back( 5 );
                indexMesh.push_back( 0 );
                }
                continue; 
            case 0b100000: // Only bottom side
                if (pos.y != 0) {
                indexMesh.push_back( 6 );
                indexMesh.push_back( 5 );
                indexMesh.push_back( 2 );
                indexMesh.push_back( 1 );
                indexMesh.push_back( 2 );
                indexMesh.push_back( 5 );
                }
                continue;
        }
    }
    return std::make_pair(indexMesh, vertexMesh);
}

// z
bool Chunk::obsBack(unsigned int thisBlock) {
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
bool Chunk::obsFront(unsigned int thisBlock) {
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
bool Chunk::obsLeft(unsigned int thisBlock) {
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
bool Chunk::obsRight(unsigned int thisBlock) {
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
bool Chunk::obsTop(unsigned int thisBlock) {
    if (blockArray[thisBlock] == blockArray.back()) {
        return false;
    }
    return( ((blockArray[thisBlock + 1] & 0b111100000000))
             == ((blockArray[thisBlock] & 0b111100000000) + 256));
}

// y
bool Chunk::obsBottom(unsigned int thisBlock){
    if (thisBlock == 0) {
        return false;
    }
    return( ((blockArray[thisBlock - 1] & 0b111100000000))
             == ((blockArray[thisBlock] & 0b111100000000) - 256));
}

