R"MULTILINE(

uniform mat4x4  mvp;
uniform f32     time;
uniform f32     grass_max_vertex_y;

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
    v3 mP = vP;
    v3 wP = world_translation + mP;
    f32 C = 0.6366197f; // 2/pi
    if (mP.y > 0.2f)
    {
        f32 lerped_movement = mix(0.0f, 0.3f, mP.y / grass_max_vertex_y);
        wP.x += lerped_movement * (sin(2.0f * C * (wP.x - time)) + 1);
    }
    
    v4 result_pos = v4(wP, 1.0f);
    fP  = wP;
    fN  = vN;
    fUV = vUV;
    fC  = vC;

    gl_Position = mvp * result_pos;
}

)MULTILINE"
