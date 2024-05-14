#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

// SELF NOTE:
// NOTE: Improve noise.

/*
    Resources on noise generation:
    
    https://openaccess.thecvf.com/content/ACCV2022/papers/Lee_NoiseTransfer_Image_Noise_Generation_with_Contrastive_Embeddings_ACCV_2022_paper.pdf

    - to be added -
 */
glm::vec2 gradient(int ix, int iy);

float dotGradient(int ix, int iy, float fx, float fy);

float interpolate(float a0, float a1, float w);

float perlin(float fx, float fy);
