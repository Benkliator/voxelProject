#include "../lib/perlin.h"

glm::vec2 gradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2;
    unsigned a = ix, b = iy;
    a *= 3284157443;

    b ^= a << s | a >> w - s;
    b *= 1911520717;

    a ^= b << s | b >> w - s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

    // Create the vector from the angle
    glm::vec2 v;
    v.x = sin(random);
    v.y = cos(random);

    return v;
}

float dotGradient(int ix, int iy, float fx, float fy) {
    // Get gradient from integer coordinates
    glm::vec2 grad = gradient(ix, iy);

    // Compute the distance vector
    float dx = fx - (float)ix;
    float dy = fy - (float)iy;

    // Compute the dot-product
    return (dx * grad.x + dy * grad.y);
}

float interpolate(float a0, float a1, float w) {
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

float perlin(float fx, float fy) {
    if (fx < 0) fx *= (-1);
    if (fy < 0) fy *= (-1);
    // Determine grid cell corner coordinates
    int x0 = (int)fx;
    int y0 = (int)fy;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Compute Interpolation weights
    float sx = fx - (float)x0;
    float sy = fy - (float)y0;

    // Compute and interpolate top two corners
    float n0 = dotGradient(x0, y0, fx, fy);
    float n1 = dotGradient(x1, y0, fx, fy);
    float ix0 = interpolate(n0, n1, sx);

    // Compute and interpolate bottom two corners
    n0 = dotGradient(x0, y1, fx, fy);
    n1 = dotGradient(x1, y1, fx, fy);
    float ix1 = interpolate(n0, n1, sx);

    // Final step: interpolate between the two previously interpolated values, now in y
    float value = interpolate(ix0, ix1, sy);

    if (value < 0) value *= (-1);
    return value * 10;
}
