#ifndef MATH_H
 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */


//
// v2
//
struct v2 { 
    union {
        struct {
            r32 x, y;
        };
        r32 E[2];
    };
};

inline v2
operator*(r32 A, v2 B) {
    v2 result;
    result.x = A * B.x;
    result.y = A * B.y;

    return result;
}

inline v2
operator+(v2 A, v2 B) {
    v2 result;
    result.x = A.x + B.x;
    result.y = A.y + B.y;

    return result;
}

inline v2
operator-(v2 A, v2 B) {
    v2 result;
    result.x = A.x - B.x;
    result.y = A.y - B.y;

    return result;
}

inline v2&
operator+=(v2& a, v2 b) {
    a.x += b.x;
    a.y += b.y;

    return a;
}

inline v2&
operator-=(v2& a, v2 b) {
    a.x -= b.x;
    a.y -= b.y;

    return a;
}

inline v2&
operator*=(v2& a, r32 b) {
    a.x *= b;
    a.y *= b;

    return a;
}

inline r32
Inner(v2 a, v2 b) {
    r32 result = a.x * b.x + a.y * b.y;
    return result;
}

inline v2
Hadamard(v2 A, v2 B) {
    v2 result = {
        A.x * B.x,
        A.y * B.y,
    };

    return result;
}

inline r32
LenSquare(v2 A) {
    r32 result = Inner(A, A);
    return result;
}

inline r32
InvLenSquare(v2 A) {
    r32 result = 1.0f / Inner(A, A);
    return result;
}

inline r32
Len(v2 A) {
    r32 result = sqrt(LenSquare(A));
    return result;
}

//
// v3
//
struct v3 { 
    union {
        struct {
            r32 x, y, z;
        };
        r32 E[3];
    };
};

inline v3
operator*(r32 A, v3 B) {
    v3 result;
    result.x = A * B.x;
    result.y = A * B.y;
    result.z = A * B.z;

    return result;
}

inline v3
operator+(v3 A, v3 B) {
    v3 result;
    result.x = A.x + B.x;
    result.y = A.y + B.y;
    result.z = A.z + B.z;

    return result;
}

inline v3
operator-(v3 A, v3 B) {
    v3 result;
    result.x = A.x - B.x;
    result.y = A.y - B.y;
    result.z = A.z - B.z;

    return result;
}

inline v3&
operator+=(v3& a, v3 b) {
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;

    return a;
}

inline v3&
operator-=(v3& a, v3 b) {
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;

    return a;
}

inline v3&
operator*=(v3& a, r32 b) {
    a.x *= b;
    a.y *= b;
    a.z *= b;

    return a;
}

inline r32
Inner(v3 a, v3 b) {
    r32 result =
        a.x * b.x +
        a.y * b.y +
        a.z * b.z;

    return result;
}

inline v3
Hadamard(v3 A, v3 B) {
    v3 result = {
        A.x * B.x,
        A.y * B.y,
        A.z * B.z 
    };

    return result;
}

inline r32
LenSquare(v3 A) {
    r32 result = Inner(A, A);
    return result;
}

inline r32
Len(v3 A) {
    r32 result = sqrt(LenSquare(A));
    return result;
}

//
// v4
//
struct v4 { 
    union {
        struct {
            r32 x, y, z, w;
        };
        struct {
            union {
                struct {
                    r32 r, g, b;
                };
                v3 rgb;
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
    v2 cen;
    v2 dim;
};

struct Rect3 {
    v3 cen;
    v3 dim;
};

b32 IsPointInRect(v3 point, Rect3 rect) {
    v3 min = rect.cen - 0.5f * rect.dim;
    v3 max = rect.cen + 0.5f * rect.dim;
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
