R"MULTILINE(

uniform mat4x4      mvp;
uniform f32         time;

// vertex info.
layout (location = 0) in v3 vP;
layout (location = 1) in v3 vN;
layout (location = 2) in v2 vUV;
layout (location = 3) in v4 vC;

layout (location = 6) in m4x4 world_transform;

smooth out v3 fP;
smooth out v3 fN;
smooth out v2 fUV;
smooth out v4 fC;

void main()
{
    // sRGB
    vec4 vertex_color = vC;
    vertex_color.r *= vertex_color.r;
    vertex_color.g *= vertex_color.g;
    vertex_color.b *= vertex_color.b;

    v3 mP = vP;
    v4 wP = world_transform * v4(mP, 1.0f);
    v4 mC = vertex_color;

    v4 result_pos = wP;
    fP  = wP.xyz;
    fN  = vN;
    fUV = vUV;
    fC  = mC;

    gl_Position = mvp * result_pos;
}

)MULTILINE"
