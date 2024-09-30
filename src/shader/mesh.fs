R"MULTILINE(

uniform sampler2D   texture_sampler;
uniform v3          cam_pos;

// material for mesh.
uniform v3 color_ambient;
uniform v3 color_diffuse;
uniform v3 color_specular;

uniform v3 DEBUG_light_P;
uniform v3 DEBUG_light_color;
uniform f32 DEBUG_light_strength;

uniform u32 octree_resolution;

uniform m4x4  voxel_VP;

smooth in v3 fP;
smooth in v3 fN;
smooth in v2 fUV;
smooth in v4 fC;

out v4 C;

uniform layout(binding = 0, r32ui) uimageBuffer DEBUG_buffer;

v3 cones[6] = {
    v3(0, 1, 0),
    v3(0, 0.5, 0.866025),
    v3(0.823639, 0.5, 0.267617),
    v3(0.509037, 0.5,-0.700629),
    v3(-0.509037, 0.5,-0.700629),
    v3(-0.823639, 0.5, 0.267617)
};


void main()
{
#if 0
    v4 albedo = texture(texture_sampler, fUV) * fC;

    //
    //
    //



    #define SHADOW_STR 1.0f
    f32 max_dist = distance(DEBUG_light_P, fP);
    v3 voxel_size = v3(octree_resolution);
    f32 occlusion = 0.0f;
    f32 march = 0.001f;

    while (march < max_dist &&
           occlusion < 1.0f)
    {
        v3 cen = fP + to_light * march;

        v3 idx_01 = (voxel_VP * v4(cen, 1)).xyz * 0.5f + v3(0.5f);
        iv3 idx = iv3(voxel_size * idx_01);
        u32 val = imageLoad(DEBUG_buffer,
                            s32(idx.x + idx.y*octree_resolution + idx.z*octree_resolution*octree_resolution)).r;
        v4 S = rgba8_to_v4(val);
        if (S.w > 0.0f)
        {
            occlusion += (1 - occlusion) * smoothstep(0.0f, max_dist, sqrt(march) * SHADOW_STR);
        }

        march += 0.1f;
    }

#if 1
    C = (texture(texture_sampler, fUV) * fC) * v4(light_sum, 1);
    C.xyz *= (1 - occlusion);
#else
    C = v4(v3(1 - occlusion), 1);
#endif




    if (C.a == 0.0f)
    {
        discard;
    }
#endif
}

)MULTILINE"
