R"MULTILINE(

uniform mat4x4      mvp;
uniform f32         time;
uniform f32         grass_max_vertex_y;
uniform sampler2D   turbulence_map;

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
    v3 mP = vP;
    v4 mC = vC;
    v4 wP = world_transform * v4(mP, 1.0f);
    f32 C = 0.6366197f; // 2/pi
    f32 lerped_movement = mix(0.0f, 0.6f, mP.y / grass_max_vertex_y);

    f32 noise_period = 1.0f / 10.0f;
    v2 noise_uv = v2(wP.x, wP.z) * noise_period;
    f32 noise = texture(turbulence_map, noise_uv).x;
    f32 turbulence = sin(time + wP.z + wP.x + noise * 4.0f) * 0.5f + 0.5f;

#if 1
    wP.x -= lerped_movement * turbulence;
#endif

#if 0
    f32 gray = turbulence;
    mC = v4(gray, gray, gray, 1.0f);
#endif
    
    v4 result_pos = wP;
    fP  = wP.xyz;
    fN  = vN;
    fUV = vUV;
    fC  = mC;

    gl_Position = mvp * result_pos;
}

)MULTILINE"
