#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <gu.h>

typedef struct Vec3f {
    float x;
    float y;
    float z;
} Vec3f;

static inline Vec3f add(Vec3f a, Vec3f b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

static inline void addTo(Vec3f* a, Vec3f b) {
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}

static inline Vec3f sub(Vec3f a, Vec3f b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

static inline void subTo(Vec3f* a, Vec3f b) {
    a->x -= b.x;
    a->y -= b.y;
    a->z -= b.z;
}

static inline Vec3f mul(Vec3f v, float s) {
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return v;
}

static inline void mulTo(Vec3f* v, float s) {
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

static inline float length(Vec3f v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline Vec3f normalize(Vec3f v) {
    return mul(v, 1.0f / length(v));
}

static inline void normalizeTo(Vec3f* v) {
    mulTo(v, 1.0f / length(*v));
}

#endif
