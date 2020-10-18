#ifndef MATH_H
#define MATH_H

typedef struct Vec3d {
    float x;
    float y;
    float z;
} Vec3d;

Vec3d add(Vec3d a, Vec3d b);
void addTo(Vec3d* a, Vec3d b);
Vec3d mul(Vec3d v, float s);

#endif
