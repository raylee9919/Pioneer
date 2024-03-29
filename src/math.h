#ifndef MATH_H
 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */


//
// vec2
//
struct vec2 { 
    union {
        struct {
            r32 x, y;
        };
        r32 E[2];
    };
};

inline vec2
operator*(r32 A, vec2 B) {
    vec2 result;
    result.x = A * B.x;
    result.y = A * B.y;

    return result;
}

inline vec2
operator+(vec2 A, vec2 B) {
    vec2 result;
    result.x = A.x + B.x;
    result.y = A.y + B.y;

    return result;
}

inline vec2
operator-(vec2 A, vec2 B) {
    vec2 result;
    result.x = A.x - B.x;
    result.y = A.y - B.y;

    return result;
}

inline vec2&
operator+=(vec2& a, vec2 b) {
    a.x += b.x;
    a.y += b.y;

    return a;
}

inline vec2&
operator-=(vec2& a, vec2 b) {
    a.x -= b.x;
    a.y -= b.y;

    return a;
}

inline vec2&
operator*=(vec2& a, r32 b) {
    a.x *= b;
    a.y *= b;

    return a;
}

inline r32
Inner(vec2 a, vec2 b) {
    r32 result = a.x * b.x + a.y * b.y;
    return result;
}

inline vec2
Hadamard(vec2 A, vec2 B) {
    vec2 result = {
        A.x * B.x,
        A.y * B.y,
    };

    return result;
}

inline r32
LenSquare(vec2 A) {
    r32 result = Inner(A, A);
    return result;
}

inline r32
InvLenSquare(vec2 A) {
    r32 result = 1.0f / Inner(A, A);
    return result;
}

inline r32
Len(vec2 A) {
    r32 result = sqrt(LenSquare(A));
    return result;
}

//
// vec3
//
struct vec3 { 
    union {
        struct {
            r32 x, y, z;
        };
        r32 E[3];
    };
};

inline vec3
operator*(r32 A, vec3 B) {
    vec3 result;
    result.x = A * B.x;
    result.y = A * B.y;
    result.z = A * B.z;

    return result;
}

inline vec3
operator+(vec3 A, vec3 B) {
    vec3 result;
    result.x = A.x + B.x;
    result.y = A.y + B.y;
    result.z = A.z + B.z;

    return result;
}

inline vec3
operator-(vec3 A, vec3 B) {
    vec3 result;
    result.x = A.x - B.x;
    result.y = A.y - B.y;
    result.z = A.z - B.z;

    return result;
}

inline vec3&
operator+=(vec3& a, vec3 b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;

    return a;
}

inline vec3&
operator-=(vec3& a, vec3 b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;

    return a;
}

inline vec3&
operator*=(vec3& a, r32 b) {
    a.x *= b;
    a.y *= b;
    a.z *= b;

    return a;
}

inline r32
Inner(vec3 a, vec3 b) {
    r32 result =
        a.x * b.x +
        a.y * b.y +
        a.z * b.z;

    return result;
}

inline vec3
Hadamard(vec3 A, vec3 B) {
    vec3 result = {
        A.x * B.x,
        A.y * B.y,
        A.z * B.z 
    };

    return result;
}

inline r32
LenSquare(vec3 A) {
    r32 result = Inner(A, A);
    return result;
}

inline r32
Len(vec3 A) {
    r32 result = sqrt(LenSquare(A));
    return result;
}

//
// vec4
//
struct vec4 { 
    union {
        struct {
            r32 x, y, z, w;
        };
        struct {
            union {
                struct {
                    r32 r, g, b;
                };
                vec3 rgb;
            };
            r32 a;
        };
        r32 E[4];
    };
};

//
// Rect.
//
struct Rect2 {
    vec2 cen;
    vec2 dim;
};

struct Rect3 {
    vec3 cen;
    vec3 dim;
};

b32 IsPointInRect(vec3 point, Rect3 rect) {
    vec3 min = rect.cen - 0.5f * rect.dim;
    vec3 max = rect.cen + 0.5f * rect.dim;
    b32 isIn = 
        min.x < point.x && max.x > point.x &&
        min.y < point.y && max.y > point.y &&
        min.z < point.z && max.z > point.z;
    return isIn;
}

//
// Misc.
//

inline r32
Square(r32 val) {
    r32 result = val * val;
    return result;
}

inline r32
Abs(r32 val) {
    r32 result = (val > 0) ? val : -val;
    return result;
}

inline r32
Lerp(r32 A, r32 B, r32 t) {
    r32 result = t * B + (1.0f - t) * A;
    return result;
}

inline r32
Clamp(r32 x, r32 low, r32 high) {
    r32 result = x;
    if (x < low) {
        result = low;
    } else if (x > high) {
        result = high;
    }
    return result;
}

inline s32
Clamp(s32 x, s32 low, s32 high) {
    s32 result = x;
    if (x < low) {
        result = low;
    } else if (x > high) {
        result = high;
    }
    return result;
}

#define MATH_H
#endif
