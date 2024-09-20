R"MULTILINE(

uniform sampler2D   texture_sampler;
uniform v3          cam_pos;

// material for mesh.
uniform v3 color_ambient;
uniform v3 color_diffuse;
uniform v3 color_specular;
uniform v3 light_pos;

uniform m4x4  voxel_VP;

smooth in v3 fP;
smooth in v3 fN;
smooth in v2 fUV;
smooth in v4 fC;

out v4 C;


layout(binding = 0, rgba8) uniform volatile coherent image3D voxel_map;
layout(binding = 1, r32ui) uniform volatile coherent uimage3D albedo_map;
layout(binding = 2, r32ui) uniform volatile coherent uimage3D normal_map;

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
    v4 albedo = texture(texture_sampler, fUV) * fC;

    v3 light_color      = v3(1.0f, 1.0f, 1.0f) * 2.0f;
    f32 light_diffuse   = 0.5f;
    f32 light_specular  = 1.0f;
    v3 light_sum;
    v3 to_light         = normalize(light_pos - fP);
    v3 from_cam         = normalize(fP - cam_pos);

    // Ambient
    v3 ambient_light = v3(0, 0, 0);

    // Distance Falloff
    f32 attenuation = light_attenuation(fP, light_pos);

    // Diffuse
    f32 cos_falloff = max(dot(fN, to_light), 0.0f);
    v3 diffuse_light = cos_falloff * color_diffuse * light_diffuse * light_color * attenuation;

    // Specular
    v3 ref = normalize(from_cam - 2 * dot(fN, from_cam) * fN);
    f32 cos_ref = max(dot(ref, to_light), 0.0f);
    cos_ref *= cos_ref;
    v3 specular_light = cos_ref * color_specular * light_specular * light_color;

    light_sum = (ambient_light + diffuse_light + specular_light);

    //
    //
    //



    f32 MAX_DIST = 2.0f;
    #define SHADOW_STR 2.0f
    v3 voxel_size = imageSize(albedo_map);
    f32 occlusion = 0.0f;
    f32 march = 0.001f;

    while (march < MAX_DIST &&
           occlusion < 1.0f)
    {
        v3 cen = fP + to_light * march;

        v3 idx_01 = (voxel_VP * v4(cen, 1)).xyz * 0.5f + v3(0.5f);
        iv3 idx = iv3(voxel_size * idx_01);
        u32 val = imageLoad(albedo_map, idx).x;
        v4 S = rgba8_to_v4(val);
        if (S.w > 0.0f)
        {
            occlusion += (1 - occlusion) * smoothstep(0.0f, MAX_DIST, sqrt(march) * SHADOW_STR);
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
}

)MULTILINE"
