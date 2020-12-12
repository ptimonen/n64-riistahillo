#ifndef MATH_UTIL_H
#define MATH_UTIL_H

#include <gu.h>

typedef struct Vec2f {
    float x;
    float y;
    float z;
} Vec2f;

static inline Vec2f add2f(Vec2f a, Vec2f b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

static inline void addTo2f(Vec2f* a, Vec2f b) {
    a->x += b.x;
    a->y += b.y;
}

static inline Vec2f sub2f(Vec2f a, Vec2f b) {
    a.x -= b.x;
    a.y -= b.y;
    return a;
}

static inline void subTo2f(Vec2f* a, Vec2f b) {
    a->x -= b.x;
    a->y -= b.y;
}

static inline Vec2f mul2f(Vec2f v, float s) {
    v.x *= s;
    v.y *= s;
    return v;
}

static inline void mulTo2f(Vec2f* v, float s) {
    v->x *= s;
    v->y *= s;
}

static inline float length2f(Vec2f v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

static inline Vec2f normalize2f(Vec2f v) {
    return mul2f(v, 1.0f / length2f(v));
}

static inline void normalizeTo2f(Vec2f* v) {
    mulTo2f(v, 1.0f / length2f(*v));
}

static inline float dot2f(Vec2f a, Vec2f b) {
    return a.x * b.x + a.y * b.y;
}

typedef struct Vec3f {
    float x;
    float y;
    float z;
} Vec3f;

static inline Vec3f add3f(Vec3f a, Vec3f b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;
    return a;
}

static inline void addTo3f(Vec3f* a, Vec3f b) {
    a->x += b.x;
    a->y += b.y;
    a->z += b.z;
}

static inline Vec3f sub3f(Vec3f a, Vec3f b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;
    return a;
}

static inline void subTo3f(Vec3f* a, Vec3f b) {
    a->x -= b.x;
    a->y -= b.y;
    a->z -= b.z;
}

static inline Vec3f mul3f(Vec3f v, float s) {
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return v;
}

static inline void mulTo3f(Vec3f* v, float s) {
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

static inline float length3f(Vec3f v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline Vec3f normalize3f(Vec3f v) {
    return mul3f(v, 1.0f / length(v));
}

static inline void normalizeTo3f(Vec3f* v) {
    mulTo3f(v, 1.0f / length(*v));
}

static inline float clamp(float x, float min, float max) {
    return MIN(max, MAX(min, x));
}

static inline float radToDeg(float radians) {
   return radians / 3.141592f * 180.0f;
}

static inline float smoothstep(float x, float min, float max) {
    x = clamp((x - min) / (max - min), 0.0, 1.0);
    return x * x * (3 - 2 * x);
}

#endif
