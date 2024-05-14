R"MULTILINE(

uniform mat4x4  mvp;

// vertex info.
layout (location = 0) in v3 vP;
layout (location = 1) in v3 vN;
layout (location = 2) in v2 vUV;
layout (location = 3) in v4 vC;

layout (location = 6) in v3 world_translation;

smooth out v3 fP;
smooth out v3 fN;
smooth out v2 fUV;
smooth out v4 fC;

void main()
{
    v4 result_pos = v4(world_translation + vP, 1.0f);
    fP  = result_pos.xyz;
    fN  = vN;
    fUV = vUV;
    fC  = vC;

    gl_Position = mvp * result_pos;
}

)MULTILINE"
