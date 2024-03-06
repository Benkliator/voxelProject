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
            visibleChunks.push_back(Chunk{glm::vec3( x          , 0,  z          )});
            visibleChunks.push_back(Chunk{glm::vec3(-x          , 0, -z          )});
            visibleChunks.push_back(Chunk{glm::vec3(-x          , 0,  z          )});
            visibleChunks.push_back(Chunk{glm::vec3( x          , 0, -z          )});

            visibleChunks.push_back(Chunk{glm::vec3( x          , 0,  z - 1      )});
            visibleChunks.push_back(Chunk{glm::vec3( x - 1      , 0, -z          )});
            visibleChunks.push_back(Chunk{glm::vec3(-x          , 0,  z - 1      )});
            visibleChunks.push_back(Chunk{glm::vec3( x - 1      , 0,  z          )});
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
