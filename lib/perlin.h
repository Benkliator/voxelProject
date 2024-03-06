#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <algorithm>

// NOTE: This noise is absolute garbage.
// TODO: Implement good noise.

glm::vec2 gradient(int ix, int iy);

float dotGradient(int ix, int iy, float fx, float fy);

float interpolate(float a0, float a1, float w);

float perlin(float fx, float fy);
