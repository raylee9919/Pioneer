/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

#define pi32                3.14159265359f
#define epsilon_f32         1.19209e-07


//
// @misc
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
    f32 result = b * t + (1 - t) * a;
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
_v2_(f32 x, f32 y)
{
    v2 v = {};
    v.x = x;
    v.y = y;
    return v;
}

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

inline v2
arm2(f32 T)
{
    v2 result = _v2_(cos(T), sin(T));
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
_v3_(f32 x, f32 y, f32 z)
{
    v3 v = {};
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

inline v3
_v3_(v2 xy, f32 z)
{
    v3 v = {};
    v.x = xy.x;
    v.y = xy.y;
    v.z = z;
    return v;
}

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
_v4_(f32 r, f32 g, f32 b, f32 a)
{
    v4 v = {};
    v.r = r;
    v.g = g;
    v.b = b;
    v.a = a;
    return v;
}

inline v4
_v4_(v3 rgb, f32 a)
{
    v4 v = {};
    v.rgb = rgb;
    v.a = a;
    return v;
}

//
// @quaternion
//
union qt
{
    struct
    {
        f32 w, x, y, z;
    };
    struct
    {
        f32 r, i, j, k;
    };
};

static qt
_qt_(f32 w, f32 x, f32 y, f32 z)
{
    qt q = {};
    q.w = w;
    q.x = x;
    q.y = y;
    q.z = z;
    return q;
}

static qt
_qt_(f32 w, v3 v)
{
    qt q = {};
    q.w = w;
    q.x = v.x;
    q.y = v.y;
    q.z = v.z;
    return q;
}

inline qt
operator + (qt a, qt b)
{
    qt q = {};
    q.w = a.w + b.w;
    q.x = a.x + b.x;
    q.y = a.y + b.y;
    q.z = a.z + b.z;
    return q;
}

inline qt
operator * (qt a, qt b)
{
    qt q = {};
    q.w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z); 
    q.x = (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y); 
    q.y = (a.w * b.y) + (a.y * b.w) + (a.z * b.x) - (a.x * b.z); 
    q.z = (a.w * b.z) + (a.z * b.w) + (a.x * b.y) - (a.y * b.x); 
    return q;
}

inline qt
operator * (qt a, f32 b)
{
    qt q = {};
    q.w = a.w * b;
    q.x = a.x * b;
    q.y = a.y * b;
    q.z = a.z * b;
    return q;
}

inline qt
operator * (f32 b, qt a)
{
    qt q = {};
    q.w = a.w * b;
    q.x = a.x * b;
    q.y = a.y * b;
    q.z = a.z * b;
    return q;
}

inline qt
operator - (qt in)
{
    qt q = {};
    q.w = -in.w;
    q.x = -in.x;
    q.y = -in.y;
    q.z = -in.z;
    return q;
}

inline f32
dot(qt a, qt b)
{
    f32 result = ( (a.w * b.w) +
                   (a.x * b.x) +
                   (a.y * b.y) +
                   (a.z * b.y) );
    return result;
}

static qt
slerp(qt q1, f32 t, qt q2)
{
    qt result;

    f32 cosom = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
    qt q3 = q2;
    if (cosom < 0.0f)
    {
        cosom = -cosom;
        q3.w = -q3.w;
        q3.x = -q3.x;
        q3.y = -q3.y;
        q3.z = -q3.z;
    }

    f32 sclp, sclq;
#if 0
    f32 threshold = epsilon_f32;
#else
    f32 threshold = 0.8f;
#endif

    if (1.0f - cosom > threshold)
    {
        f32 omega, sinom;
        omega = acos(cosom);
        sinom = sin(omega);
        sclp  = sin((1.0f - t) * omega) / sinom;
        sclq  = sin(t * omega) / sinom;
    } 
    else
    {
        sclp = 1.0f - t;
        sclq = t;
    }

    result.x = sclp * q1.x + sclq * q3.x;
    result.y = sclp * q1.y + sclq * q3.y;
    result.z = sclp * q1.z + sclq * q3.z;
    result.w = sclp * q1.w + sclq * q3.w;

    return result;
}

//
// @m4x4
//

// IMPORTANT: row-major!
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
    m4x4 result = m;
    result.e[0][3] += t.x;
    result.e[1][3] += t.y;
    result.e[2][3] += t.z;
    return result;
}

static m4x4
to_m4x4(qt q) 
{
    m4x4 result = identity();
    f32 w = q.w;
    f32 x = q.x;
    f32 y = q.y;
    f32 z = q.z;

    result.e[0][0] = 1.0f - 2.0f * (y * y + z * z);
    result.e[0][1] = 2.0f * (x * y - z * w);
    result.e[0][2] = 2.0f * (x * z + y * w);

    result.e[1][0] = 2.0f * (x * y + z * w);
    result.e[1][1] = 1.0f - 2.0f * (x * x + z * z);
    result.e[1][2] = 2.0f * (y * z - x * w);

    result.e[2][0] = 2.0f * (x * z - y * w);
    result.e[2][1] = 2.0f * (y * z + x * w);
    result.e[2][2] = 1.0f - 2.0f * (x * x + y * y);

    return result;
}

static m4x4
scale(m4x4 m, v3 s) 
{
    m4x4 r = m;
    r.e[0][0] *= s.x;
    r.e[1][1] *= s.y;
    r.e[2][2] *= s.z;
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
// @Rect
//
INTROSPECT(category:"math") struct Rect2 
{
    v2 min;
    v2 max;
};

inline Rect2
rect2_min_max(v2 min, v2 max)
{
    Rect2 result = {};
    result.min = min;
    result.max = max;
    return result;
}

inline Rect2
rect2_cen_half_dim(v2 cen, v2 h_dim)
{
    Rect2 result = {};
    result.min = cen - h_dim;
    result.max = cen + h_dim;
    return result;
}

inline Rect2
rect2_inv_inf()
{
    Rect2 result = {};
    result.min.x =  F32_MAX;
    result.min.y =  F32_MAX;
    result.max.x = -F32_MAX;
    result.max.y = -F32_MAX;
    return result;
}

inline Rect2
offset(Rect2 rect, v2 offset)
{
    Rect2 result = {};
    result.min = rect.min + offset;
    result.max = rect.max + offset;
    return result;
}

inline Rect2
add_radius_to(Rect2 rect, v2 radius)
{
    Rect2 result = rect;
    rect.min -= radius;
    rect.max += radius;
    return result;
}

INTROSPECT(category:"math") struct Rect3 
{
    v3 cen;
    v3 dim;
};

inline b32
is_in_rect(Rect2 rect, v2 p)
{
    b32 result = (rect.min.x <= p.x && 
                  rect.min.y <= p.y && 
                  rect.max.x > p.x && 
                  rect.max.y > p.y);
    return result;
}

inline b32 
in_rect(v3 point, Rect3 rect) 
{
    v3 min = rect.cen - 0.5f * rect.dim;
    v3 max = rect.cen + 0.5f * rect.dim;
    b32 is_in =  (min.x < point.x && max.x > point.x &&
                  min.y < point.y && max.y > point.y &&
                  min.z < point.z && max.z > point.z);
    return is_in;
}

inline v2
get_dim(Rect2 rect)
{
    v2 result = {};
    result.x = (rect.max.x - rect.min.x);
    result.y = (rect.max.y - rect.min.y);
    return result;
}

inline f32
get_width(Rect2 rect)
{
    f32 result = (rect.max.x - rect.min.x); 
    return result;
}

inline f32
get_height(Rect2 rect)
{
    f32 result = (rect.max.y - rect.min.y); 
    return result;
}

inline m4x4
trs_to_transform(v3 translation, qt rotation, v3 scaling)
{
    m4x4 T = translate(identity(), translation);
    m4x4 R = to_m4x4(rotation);
    m4x4 S = scale(identity(), scaling);
    m4x4 result = T * R * S;
    return result;
}

inline qt 
rotate(qt q, v3 axis, f32 t)
{
    f32 c = cos(t * 0.5f);
    f32 s = sin(t * 0.5f);
    v3 n = s * normalize(axis);
    qt result = _qt_(c, n.x, n.y, n.z) * q;
    return result;
}

struct v3i
{
    s32 x, y, z;
};

inline v3i
_v3i_(s32 x, s32 y, s32 z)
{
    v3i v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

//
//
//


