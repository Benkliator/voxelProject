#include "player.h"

Player::Player(World* w) : world{ w } {
    worldCam = new Camera{};
    skyCam = new Camera{};
}

Player::~Player() {
    delete worldCam;
    delete skyCam;
}

void Player::moveMouse(GLFWwindow* window, float xi, float yi) {
    worldCam->processMouseMovement(window, xi, yi);
    skyCam->processMouseMovement(window, xi, yi);

    view();
}

void Player::movePlayer(GLFWwindow* window, float dt) {
    // only worldcam because skybox moves with player
    float speed = 12.0f * dt;
    pos = worldCam->processKeyboardInput(window, speed);
    view();
}

// Make less scuffed in future lol
void Player::breakBlock() {
    unsigned xChunk = viewBlock.x - (viewBlock.x % 16);
    unsigned zChunk = viewBlock.z - (viewBlock.z % 16);

    world->getChunk(xChunk, 0, zChunk)
        ->removeBlock(viewBlock.x % 16, viewBlock.y, viewBlock.z % 16);

    view();
}

// NOTE: this works but is sub-optimal
void Player::placeBlock() {
    auto [from, to] = worldCam->rayCast(5.0f);

    unsigned xBlock = unsigned(std::round(to.x));
    unsigned yBlock = unsigned(std::round(to.y));
    unsigned zBlock = unsigned(std::round(to.z));

    unsigned xChunk = xBlock - (xBlock % 16);
    unsigned zChunk = zBlock - (zBlock % 16);

    float it = 1.0f;
    while(!isAir(world->getChunk(xChunk, 0, zChunk)
                ->getBlock(xBlock % 16, yBlock, zBlock % 16)) 
            && it <= 4.0f) {
        auto [from, to] = worldCam->rayCast(5.0f - it);

        xBlock = unsigned(std::round(to.x));
        yBlock = unsigned(std::round(to.y));
        zBlock = unsigned(std::round(to.z));

        xChunk = xBlock - (xBlock % 16);
        zChunk = zBlock - (zBlock % 16);
        it++;
    }
    if (it != 4){
        auto [from, to] = worldCam->rayCast(5.0f - it);
        world->getChunk(xChunk, 0, zChunk)->placeBlock(xBlock % 16, yBlock, zBlock % 16);
        view();
    }
}

glm::mat4 Player::worldLook() {
    return worldCam->lookAt();
}

glm::mat4 Player::skyLook() {
    return skyCam->lookAt();
}

// NOTE: better raycasting?
void Player::view() {
    auto [from, to] = worldCam->rayCast(1.0f);

    unsigned xBlock = unsigned(std::round(to.x));
    unsigned yBlock = unsigned(std::round(to.y));
    unsigned zBlock = unsigned(std::round(to.z));

    unsigned xChunk = xBlock - (xBlock % 16);
    unsigned zChunk = zBlock - (zBlock % 16);
    float it = 1.0f;
    while(isAir(world->getChunk(xChunk, 0, zChunk)
                ->getBlock(xBlock % 16, yBlock, zBlock % 16)) 
            && it <= 4.0f) {
        auto [from, to] = worldCam->rayCast(1.0f + it);

        xBlock = unsigned(std::round(to.x));
        yBlock = unsigned(std::round(to.y));
        zBlock = unsigned(std::round(to.z));

        xChunk = xBlock - (xBlock % 16);
        zChunk = zBlock - (zBlock % 16);
        it++;
    }
    Chunk* prevChunk = world->getChunk(viewBlock.x - (viewBlock.x % 16), 
            0, 
            viewBlock.z - (viewBlock.z % 16));
    // Unhighlight viewblock
    if (prevChunk) {
        prevChunk->unhighlightBlock(viewBlock.x % 16, viewBlock.y, viewBlock.z % 16);
        prevChunk->clearMesh();
        prevChunk->generateMesh();
    }
    Chunk* temp = world->getChunk(xChunk, 0, zChunk);
    if (!isAir(temp->getBlock(xBlock % 16, yBlock, zBlock % 16))) {
        // Highlight new viewblock
        temp->highlightBlock(xBlock % 16, yBlock, zBlock % 16);
        // Reload mesh
        temp->clearMesh();
        temp->generateMesh();
        // Assign new viewblock
        viewBlock = glm::uvec3(xBlock, yBlock, zBlock);
    }
}
