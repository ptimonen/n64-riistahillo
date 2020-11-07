#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <gu.h>

typedef struct Vec3d {
    float x;
    float y;
    float z;
} Vec3d;

static inline Vec3d add(Vec3d a, Vec3d b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

static inline void addTo(Vec3d* a, Vec3d b) {
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}

static inline Vec3d sub(Vec3d a, Vec3d b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

static inline void subTo(Vec3d* a, Vec3d b) {
    a->x -= b.x;
    a->y -= b.y;
    a->z -= b.z;
}

static inline Vec3d mul(Vec3d v, float s) {
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return v;
}

static inline void mulTo(Vec3d* v, float s) {
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

static inline float length(Vec3d v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline Vec3d normalize(Vec3d v) {
    return mul(v, 1.0f / length(v));
}

static inline void normalizeTo(Vec3d* v) {
    mulTo(v, 1.0f / length(*v));
}

#endif
