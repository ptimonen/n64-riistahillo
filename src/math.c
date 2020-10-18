#include "math.h"

Vec3d add(Vec3d a, Vec3d b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

void addTo(Vec3d* a, Vec3d b) {
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}

Vec3d mul(Vec3d v, float s) {
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return v;
}
