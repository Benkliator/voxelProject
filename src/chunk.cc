#include "../lib/chunk.h"
#include "../lib/BensGLUtilities.h"

Chunk::Chunk(glm::vec3 offset)
{
    // Chunk offset
    chunkPos = glm::vec3(offset.x * 16, 0 * 16, offset.z * 16);

    generateTerrain();
}

// NOTE: Might need to actually impelent this function for memory safety (lol)
Chunk::~Chunk()
{
    std::vector<unsigned long>().swap(blockArray);
}


// NOTE: This function works but is the absolute
// worst function in the entire system, it currently just
// exists as a lazy proof of concept. Instead, store adjacent chunks
// efficiently in the world class, and please dont make n^3 functions
// like these.
//
// NOTE of NOTE: I have made the function better by removing its contents.
void Chunk::cornerCheck(std::vector<Chunk>& otherChunk)
{
}

bool Chunk::findBlock(glm::vec3 blockPos)
{
    for (auto block : blockArray)
    {
        glm::vec3 pos((float)(block &                 0b1111),
                      (float)(block & 0b11111111111100000000) / 256.0f,
                      (float)(block &             0b11110000) / 16.0f);
        if (pos == blockPos) return true;
    }
    return false;
}

glm::vec3 Chunk::returnPos()
{
    return chunkPos;
}

void Chunk::generateTerrain()
{
    float freq = 0.076323343; // Frequency
    float amp = 0.75;        // Amplifier
    for (int x = 0; x < 16; x++)
    {
        for (int z = 0; z < 16; z++)
        {
            // Uses perlin function to calculate height for xz position
            float height = perlin(((x + chunkPos.x) * freq) / 16,
                                 ((z + chunkPos.z) * freq)/ 16) * amp + 1;
            for (int y = 0; y < height;
                 y++)
            {
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


std::pair< std::vector<unsigned int>, std::vector<float> > Chunk::generateMesh()
{
   std::vector<unsigned int> indexMesh;
   std::vector<float>        vertexMesh;
   // NOTE: This implementation is quite slow, but very consitent.
   // It will also be easy to modify whenever normals are to be added, however
   // it can 100% be cleaned up and become even better.
   int it = 0;
   for (auto block : blockArray)
   {
        glm::vec3 pos((float)(block &                 0b1111),
                      (float)(block & 0b11111111111100000000) / 256.0f,
                      (float)(block &             0b11110000) / 16.0f);

        Block blockType( (block & 0b111100000000000000000000) / 1048576.0f );
        if( !obsBack(it) && pos.z != 0)
        {
            indexMesh.push_back( vertexMesh.size() / 5 + 3);
            indexMesh.push_back( (vertexMesh.size() / 5));
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 2);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            for (size_t i = 0; i < 20; i+=5)
            {
                vertexMesh.push_back(backVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(backVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(backVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(backVertices[i + 3] + (blockType.side & 240)/16);
                vertexMesh.push_back(backVertices[i + 4] + (blockType.side & 15) );
            }
        }
        if( !obsFront(it) && pos.z != 15)
        {
            indexMesh.push_back( vertexMesh.size() / 5);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 2);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            for (size_t i = 0; i < 20; i+=5)
            {
                vertexMesh.push_back(frontVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(frontVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(frontVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(frontVertices[i + 3] + (blockType.side & 240)/16);
                vertexMesh.push_back(frontVertices[i + 4] + (blockType.side & 15));
            }
        }
        if( !obsLeft(it) && pos.x != 0)
        {
            indexMesh.push_back( vertexMesh.size() / 5);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 2);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            for (size_t i = 0; i < 20; i+=5)
            {
                vertexMesh.push_back(leftVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(leftVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(leftVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(leftVertices[i + 3] + (blockType.side & 240)/16);
                vertexMesh.push_back(leftVertices[i + 4] + (blockType.side & 15));
            }
        }
        if( !obsRight(it) && pos.x != 15)
        {
            indexMesh.push_back( vertexMesh.size() / 5);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 2);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            for (size_t i = 0; i < 20; i+=5)
            {
                vertexMesh.push_back(rightVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(rightVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(rightVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(rightVertices[i + 3] + (blockType.side & 240)/16);
                vertexMesh.push_back(rightVertices[i + 4] + (blockType.side & 15));
            }
        }
        if( !obsBottom(it) && pos.y != 0)
        {
            indexMesh.push_back( vertexMesh.size() / 5);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 2);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            for (size_t i = 0; i < 20; i+=5)
            {
                vertexMesh.push_back(bottomVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(bottomVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(bottomVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(bottomVertices[i + 3] + (blockType.bottom & 240)/16);
                vertexMesh.push_back(bottomVertices[i + 4] + (blockType.bottom & 15));
            }
        }
        if(!obsTop(it))
        {
            indexMesh.push_back( vertexMesh.size() / 5);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 2);
            indexMesh.push_back( (vertexMesh.size() / 5) + 1);
            indexMesh.push_back( (vertexMesh.size() / 5) + 3);
            for (size_t i = 0; i < 20; i+=5)
            {
                vertexMesh.push_back(topVertices[i]     + pos.x + chunkPos.x);
                vertexMesh.push_back(topVertices[i + 1] + pos.y + chunkPos.y);
                vertexMesh.push_back(topVertices[i + 2] + pos.z + chunkPos.z);
                vertexMesh.push_back(topVertices[i + 3] + (blockType.top & 240)/16);
                vertexMesh.push_back(topVertices[i + 4] + (blockType.top & 15));
            }
        }
        it++;
    }
    return std::make_pair(indexMesh, vertexMesh);
}

bool Chunk::obsBack(unsigned int thisBlock) // z
{
    if (thisBlock == 0) return false;
    int temp = thisBlock - 1;
    while ((blockArray[temp]           & 0b11110000)
            >= ((blockArray[thisBlock] & 0b11110000) - 16)
            && temp != -1)
    {
        if ((blockArray[temp]         & 0b111100001111) 
            == (blockArray[thisBlock] & 0b111100001111)) return true;
        temp--;
    }
    return false;
}

bool Chunk::obsFront(unsigned int thisBlock) // z
{
    if (blockArray[thisBlock] == blockArray.back()) return false;
    int temp = thisBlock + 1;
    while ((blockArray[temp]           & 0b11110000)
            <= ((blockArray[thisBlock] & 0b11110000) + 16)
            && blockArray[temp - 1] != blockArray.back())
    {
        if ((blockArray[temp]         & 0b111100001111)
            == (blockArray[thisBlock] & 0b111100001111)) return true;
        temp++;
    }
    return false;
}

bool Chunk::obsLeft(unsigned int thisBlock) // x
{
    if (thisBlock == 0) return false;
    int temp = thisBlock - 1;
    while ((blockArray[temp]           & 0b1111)
            >= ((blockArray[thisBlock] & 0b1111) - 1)
            && temp != - 1) 
    {
        if ((blockArray[temp]         & 0b111111110000)
            == (blockArray[thisBlock] & 0b111111110000)) return true;
        temp--;
    }
    return false;

}

bool Chunk::obsRight(unsigned int thisBlock) // x
{
    if (blockArray[thisBlock] == blockArray.back()) return false;
    int temp = thisBlock + 1;
    while ((blockArray[temp]          & 0b1111)
            <=((blockArray[thisBlock] & 0b1111) + 1)
            && blockArray[temp - 1] != blockArray.back())
    {
        if ((blockArray[temp]         & 0b111111110000)
            == (blockArray[thisBlock] & 0b111111110000)) return true;
        temp++;
    }
    return false;
}

// Fix edge case of top of block 0, 1, 0 in a chunk having
// the top too a block too low?
bool Chunk::obsTop(unsigned int thisBlock)
{
    if (blockArray[thisBlock] == blockArray.back()) return false;
    return( ((blockArray[thisBlock + 1] & 0b111100000000))
             == ((blockArray[thisBlock] & 0b111100000000) + 256));
}

bool Chunk::obsBottom(unsigned int thisBlock)
{
    if (thisBlock == 0) return false;
    return( ((blockArray[thisBlock - 1] & 0b111100000000))
             == ((blockArray[thisBlock] & 0b111100000000) - 256));
}

