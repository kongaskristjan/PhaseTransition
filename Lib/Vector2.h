#ifndef __VECTOR2_H__
#define __VECTOR2_H__

#include <cmath>

template<typename T>
struct Vector2 {
    T x, y;

    Vector2(): x(0), y(0) {}
    Vector2(T _x, T _y): x(_x), y(_y) {}

    Vector2& operator+=(const Vector2 &rhs) { x += rhs.x; y += rhs.y; return *this; }
    Vector2& operator-=(const Vector2 &rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    Vector2& operator*=(const T &rhs) { x *= rhs; y *= rhs; return *this; }
    Vector2& operator/=(const T &rhs) { T mul = 1. / rhs; x *= mul; y *= mul; return *this; }
    
    T magnitude2() const { return x * x + y * y; }
    T magnitude() const { return sqrt(magnitude2()); }
    Vector2 norm() const { return Vector2(*this) /= magnitude(); }
};

template<typename T> Vector2<T> operator+(const Vector2<T> &lhs, const Vector2<T> &rhs) { return Vector2<T>(lhs) += rhs; }
template<typename T> Vector2<T> operator-(const Vector2<T> &lhs, const Vector2<T> &rhs) { return Vector2<T>(lhs) -= rhs; }
template<typename T> Vector2<T> operator*(const Vector2<T> &lhs, const T &rhs) { return Vector2<T>(lhs) *= rhs; }
template<typename T> Vector2<T> operator/(const Vector2<T> &lhs, const T &rhs) { return Vector2<T>(lhs) /= rhs; }
template<typename T> Vector2<T> operator-(const Vector2<T> &v) { return Vector2<T>(-v.x, -v.y); }

template<typename T> T dotProduct(const Vector2<T> &lhs, const Vector2<T> &rhs) { return lhs.x * rhs.x + lhs.y * rhs.y; }
template<typename T> T crossProduct(const Vector2<T> &lhs, const Vector2<T> &rhs) { return lhs.x * rhs.y - lhs.y * rhs.x; }

typedef Vector2<float> Vector2F;
typedef Vector2<double> Vector2D;

#define EXPECT_VECTOR2_EQ(lhs, rhs) do {\
    EXPECT_DOUBLE_EQ((lhs).x, (rhs).x);\
    EXPECT_DOUBLE_EQ((lhs).y, (rhs).y);\
} while(false)
/*
template<typename T>
void EXPECT_VECTOR2_EQ(const Vector2<T> &lhs, const Vector2<T> &rhs) {
    EXPECT_DOUBLE_EQ(lhs.x, rhs.x);
    EXPECT_DOUBLE_EQ(lhs.y, rhs.y);
}
*/
#endif
