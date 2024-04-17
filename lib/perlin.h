#pragma once

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <cmath>

// NOTE: This noise is absolute garbage.
// TODO: Implement good noise.

glm::vec2 gradient(int ix, int iy);

float dotGradient(int ix, int iy, float fx, float fy);

float interpolate(float a0, float a1, float w);

float perlin(float fx, float fy);
