#include "gametime.h"

#include <glm/trigonometric.hpp>

GameTime::GameTime() {
}

void GameTime::incrementTime() {
    ++currentGameTime;

    if (currentGameTime >= dayLength) {
        currentGameTime = 0;
    }

    angle = glm::radians(static_cast<float>(currentGameTime) / 60);
}

GameTime::TimeState GameTime::getTimeState() {
    if (currentGameTime >= day && currentGameTime < dusk) {
        return TimeState::Day;
    } else if (currentGameTime >= dusk && currentGameTime < night) {
        return TimeState::Dusk;
    } else if (currentGameTime >= night && currentGameTime < dawn) {
        return TimeState::Night;
    } else {
        return TimeState::Dawn;
    }
}

double GameTime::getSkyboxAngle() {
    return angle;
}
