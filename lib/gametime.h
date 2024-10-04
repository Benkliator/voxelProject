#pragma once

#include <glm/fwd.hpp>
#include <sys/types.h>
#include <glm/trigonometric.hpp>

const unsigned dayLength = 8640;

const unsigned night = 0;
const unsigned dawn = 3000;
const unsigned day = 5320;
const unsigned dusk = 8640;

class GameTime {
public:
    GameTime();

    void incrementTime();

    enum TimeState : ushort {
        Day = 0,
        Night,
        Dusk,
        Dawn,
    };

    TimeState getTimeState();
    double getSkyboxAngle();

public:
    unsigned currentGameTime = 0;
    float angle = 0;
};
