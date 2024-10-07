#pragma once

#include <glm/fwd.hpp>
#include <sys/types.h>
#include <glm/trigonometric.hpp>

const unsigned dayLength = 8640;

const unsigned night = 0;
const unsigned dawn = 9 * dayLength / 20;
const unsigned day =  10 * dayLength / 20;
const unsigned dusk = 19 * dayLength / 20;

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
    unsigned getLight();

public:
    unsigned currentGameTime = 0;
    unsigned daylightStrength = 0;
    float angle = 0;
};
