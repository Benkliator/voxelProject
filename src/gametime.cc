#include "gametime.h"

#include <glm/trigonometric.hpp>

GameTime::GameTime() {
}

void GameTime::incrementTime() {
    currentGameTime += 1;

    if (currentGameTime >= dayLength) {
        currentGameTime = 0;
    }

    angle = glm::radians(static_cast<float>(currentGameTime) / 25);
}

GameTime::TimeState GameTime::getTimeState() {
    if (currentGameTime >= night && currentGameTime < dawn) {
        return TimeState::Night;
    } else if (currentGameTime >= dawn && currentGameTime < day) {
        return TimeState::Dawn;
    } else if (currentGameTime >= day && currentGameTime < dusk) {
        return TimeState::Day;
    } else if (currentGameTime >= dusk && currentGameTime < dayLength){
        return TimeState::Dusk;
    }

    return TimeState::Day;
}

double GameTime::getSkyboxAngle() {
    return angle;
}

unsigned GameTime::getLight() {
    enum TimeState state = getTimeState();

    switch (state) {
        case TimeState::Day:
            return 10;
        case TimeState::Night:
            return 4;
    }

    return 7;
}
