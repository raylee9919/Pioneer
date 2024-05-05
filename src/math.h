 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#define pi32    3.14159265359f


//
// Misc.
//
inline f32
square(f32 val) 
{
    f32 result = val * val;
    return result;
}

inline f32
abs(f32 val) 
{
    f32 result = (val > 0) ? val : -val;
    return result;
}

inline f32
lerp(f32 a, f32 t, f32 b) 
{
    f32 result = b * t + (1 - a) * t;
    return result;
}

inline f32
clamp(f32 x, f32 low, f32 high) 
{
    f32 result = x;
    if (x < low) 
    {
        result = low;
    }
    else if (x > high) 
    {
        result = high;
    }
    return result;
}

inline s32
clamp(s32 x, s32 low, s32 high) 
{
    s32 result = x;
    if (x < low) 
    {
        result = low;
    }
    else if (x > high) 
    {
        result = high;
    }
    return result;
}

inline f32
safe_ratio(f32 a, f32 b) 
{
    f32 result = 0.0f;
    if (b != 0) 
    {
        result = a / b;
    }
    return result;
}

//
// @v2
//
union v2
{
    struct 
    {
        f32 x, y;
    };
    f32 e[2];
};

inline v2
operator-(const v2 &in) 
{
    v2 V;
    V.x = -in.x;
    V.y = -in.y;
    return V;
}

inline v2
operator*(f32 A, v2 B) 
{
    v2 result;
    result.x = A * B.x;
    result.y = A * B.y;

    return result;
}

inline v2
operator+(v2 A, v2 B) 
{
    v2 result;
    result.x = A.x + B.x;
    result.y = A.y + B.y;

    return result;
}

inline v2
operator-(v2 A, v2 B) 
{
    v2 result;
    result.x = A.x - B.x;
    result.y = A.y - B.y;

    return result;
}

inline v2&
operator+=(v2& a, v2 b) 
{
    a.x += b.x;
    a.y += b.y;

    return a;
}

inline v2&
operator-=(v2& a, v2 b) 
{
    a.x -= b.x;
    a.y -= b.y;

    return a;
}

inline v2&
operator*=(v2& a, f32 b) 
{
    a.x *= b;
    a.y *= b;

    return a;
}

inline f32
dot(v2 a, v2 b) 
{
    f32 result = a.x * b.x + a.y * b.y;
    return result;
}

inline v2
hadamard(v2 A, v2 B) 
{
    v2 result = {
        A.x * B.x,
        A.y * B.y,
    };

    return result;
}

inline f32
len_square(v2 A) 
{
    f32 result = dot(A, A);
    return result;
}

inline f32
inv_len_square(v2 A) 
{
    f32 result = 1.0f / dot(A, A);
    return result;
}

inline f32
len(v2 A) 
{
    f32 result = sqrt(len_square(A));
    return result;
}

//
// @v3
//
union v3 
{
    struct 
    {
        f32 x, y, z;
    };
    f32 e[3];
};

inline v3
operator-(const v3 &in) 
{
    v3 V;
    V.x = -in.x;
    V.y = -in.y;
    V.z = -in.z;
    return V;
}

inline v3
operator*(f32 A, v3 B) 
{
    v3 result;
    result.x = A * B.x;
    result.y = A * B.y;
    result.z = A * B.z;

    return result;
}

inline v3
operator+(v3 A, v3 B) 
{
    v3 result;
    result.x = A.x + B.x;
    result.y = A.y + B.y;
    result.z = A.z + B.z;

    return result;
}

inline v3
operator-(v3 A, v3 B) 
{
    v3 result;
    result.x = A.x - B.x;
    result.y = A.y - B.y;
    result.z = A.z - B.z;

    return result;
}

inline v3&
operator+=(v3& a, v3 b) 
{
    a.x += b.x;
    a.y += b.y;
    a.z += b.z;

    return a;
}

inline v3&
operator-=(v3& a, v3 b) 
{
    a.x -= b.x;
    a.y -= b.y;
    a.z -= b.z;

    return a;
}

inline v3&
operator*=(v3& a, f32 b) 
{
    a.x *= b;
    a.y *= b;
    a.z *= b;

    return a;
}

inline f32
dot(v3 a, v3 b) 
{
    f32 result = (a.x * b.x +
                  a.y * b.y +
                  a.z * b.z);

    return result;
}

inline v3
hadamard(v3 A, v3 B) 
{
    v3 result = {
        A.x * B.x,
        A.y * B.y,
        A.z * B.z 
    };
    return result;
}

inline v3
cross(v3 A, v3 B) 
{
    v3 R = {};
    R.x = (A.y * B.z) - (B.y * A.z);
    R.y = (A.z * B.x) - (B.z * A.x);
    R.z = (A.x * B.y) - (B.x * A.y);
    return R;
}

inline f32
len_square(v3 A) 
{
    f32 result = dot(A, A);
    return result;
}

inline f32
len(v3 A) 
{
    f32 result = sqrt(len_square(A));
    return result;
}

inline v3
normalize(v3 a) 
{
    v3 r = a;
    f32 inv_len = (1.0f / len(r));
    r *= inv_len;
    return r;
}

inline v3
lerp(v3 a, f32 t, v3 b)
{
    v3 result = {};
    result.x = lerp(a.x, t, b.x);
    result.y = lerp(a.y, t, b.y);
    result.z = lerp(a.z, t, b.z);
    return result;
}

//
// @v4
//
union v4
{
    struct 
    {
        union 
        {
            struct 
            {
                f32 r, g, b;
            };
            v3 rgb;
        };
        f32 a;
    };
    f32 e[4];
};

inline v4
V4(f32 r, f32 g, f32 b, f32 a)
{
    v4 result;
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

//
// @m4x4
//

// these are row-major, which is apposed to gl's column-major notation.
struct m4x4 
{
    f32 e[4][4];
};

inline m4x4
operator*(m4x4 a, m4x4 b) 
{
    m4x4 R = {};

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            for (int i = 0; i < 4; ++i) {
                R.e[r][c] += a.e[r][i] * b.e[i][c];
            }
        }
    }

    return R;
}

static v3
transform(m4x4 M, v3 P, f32 Pw = 1.0f) 
{
    v3 R;

    R.x = M.e[0][0] * P.x +
          M.e[0][1] * P.y +
          M.e[0][2] * P.z +
          M.e[0][3] * Pw;

    R.y = M.e[1][0] * P.x +
          M.e[1][1] * P.y +
          M.e[1][2] * P.z +
          M.e[1][3] * Pw;

    R.z = M.e[2][0] * P.x +
          M.e[2][1] * P.y +
          M.e[2][2] * P.z +
          M.e[2][3] * Pw;

    return R;
}

inline v3
operator*(m4x4 m, v3 p) 
{
    v3 r = transform(m, p, 1.0f);
    return r;
}

inline m4x4
identity() 
{
    m4x4 r = {
       {{ 1,  0,  0,  0 },
        { 0,  1,  0,  0 },
        { 0,  0,  1,  0 },
        { 0,  0,  0,  1 }},
    };
    return r;
}

inline m4x4
x_rotation(f32 a) 
{
    f32 c = cos(a);
    f32 s = sin(a);
    m4x4 r = {
       {{ 1,  0,  0,  0 },
        { 0,  c, -s,  0 },
        { 0,  s,  c,  0 },
        { 0,  0,  0,  1 }},
    };

    return r;
}

inline m4x4
y_rotation(f32 a) 
{
    f32 c = cos(a);
    f32 s = sin(a);
    m4x4 r = {
       {{ c,  0,  s,  0 },
        { 0,  1,  0,  0 },
        {-s,  0,  c,  0 },
        { 0,  0,  0,  1 }},
    };

    return r;
}

inline m4x4
z_rotation(f32 a) 
{
    f32 c = cos(a);
    f32 s = sin(a);
    m4x4 r = {
       {{ c, -s,  0,  0 },
        { s,  c,  0,  0 },
        { 0,  0,  1,  0 },
        { 0,  0,  0,  1 }}
    };

    return r;
}

inline m4x4
transpose(m4x4 m) 
{
    m4x4 r = {};
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            r.e[j][i] = m.e[i][j];
        }
    }
    return r;
}

inline m4x4
rows(v3 x, v3 y, v3 z) 
{
    m4x4 r = {
       {{ x.x, x.y, x.z,  0 },
        { y.x, y.y, y.z,  0 },
        { z.x, z.y, z.z,  0 },
        {   0,   0,   0,  1 }}
    };
    return r;
}

inline m4x4
columns(v3 x, v3 y, v3 z) 
{
    m4x4 r = {
       {{ x.x, y.x, z.x,  0 },
        { x.y, y.y, z.y,  0 },
        { x.z, y.z, z.z,  0 },
        {   0,   0,   0,  1 }}
    };
    return r;
}

static m4x4
translate(m4x4 m, v3 t) 
{
    m4x4 r = m;
    r.e[0][3] += t.x;
    r.e[1][3] += t.y;
    r.e[2][3] += t.z;
    return r;
}

inline m4x4
camera_transform(v3 x, v3 y, v3 z, v3 p) 
{
    m4x4 R = rows(x, y, z);
    R = translate(R, -(R * p));

    return R;
}

inline v3
get_row(m4x4 M, u32 R) 
{
    v3 V = {
        M.e[R][0],
        M.e[R][1],
        M.e[R][2]
    };
    return V;
}

inline v3
get_column(m4x4 M, u32 C) 
{
    v3 V = {
        M.e[0][C],
        M.e[1][C],
        M.e[2][C]
    };
    return V;
}

//
// @quaternion
//
struct quat
{
    f32 w, x, y, z;
};

inline quat
slerp(quat a, f32 t, quat b) 
{
    quat result = {};
    return result;
}


//
// @Rect
//
struct Rect2 
{
    v2 cen;
    v2 dim;
};

struct Rect3 
{
    v3 cen;
    v3 dim;
};

b32 IsPointInRect(v3 point, Rect3 rect) 
{
    v3 min = rect.cen - 0.5f * rect.dim;
    v3 max = rect.cen + 0.5f * rect.dim;
    b32 is_in =  (min.x < point.x && max.x > point.x &&
                 min.y < point.y && max.y > point.y &&
                 min.z < point.z && max.z > point.z);
    return is_in;
}


inline m4x4
trs_to_transform(v3 translation, quat rotation, v3 scaling)
{
    m4x4 result = {};

    return result;
}
