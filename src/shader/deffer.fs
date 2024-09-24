R"MULTILINE(

in v3 fP;
in v2 fUV;

out v4 C;

uniform m4x4  voxel_VP;

uniform v3 DEBUG_light_P;
uniform v3 DEBUG_light_color;
uniform f32 DEBUG_light_strength;


layout (binding = 1) uniform sampler2D   gP;
layout (binding = 2) uniform sampler2D   gN;
layout (binding = 3) uniform sampler2D   gC;


layout(binding = 0, r32ui) uniform volatile coherent uimage3D albedo_map;


void main()
{
    // Read from G-Buffer
    v4 fP_read = texture(gP, fUV);
    v3 fP = fP_read.xyz;
    v3 fN = normalize(texture(gN, fUV).xyz); // we normalized on previous pass... but for safety.
    v3 fC = texture(gC, fUV).rgb;


    //
    //
    //
    f32 light_diffuse = 1.0f;

    if (fP_read.a != 0)
    {
        v3 to_light = normalize(DEBUG_light_P - fP);

        //
        // Direct Light
        //
        // Distance Falloff
        f32 attenuation = light_attenuation(fP, DEBUG_light_P);
    
        // Diffuse
        f32 cos_falloff = max(dot(fN, to_light), 0.0f);
        v3 diffuse_light = cos_falloff * fC * light_diffuse * DEBUG_light_strength * DEBUG_light_color * attenuation;




    
        #define SHADOW_STR 1.0f
        v3 voxel_size = imageSize(albedo_map);
        f32 occlusion = 0.0f;
        f32 march = 0.001f;

        f32 max_dist = distance(DEBUG_light_P, fP);

        while (march < max_dist &&
               occlusion < 1.0f)
        {
            v3 cen = fP + to_light * march;

            v3 idx_01 = (voxel_VP * v4(cen, 1)).xyz * 0.5f + v3(0.5f);
            iv3 idx = iv3(voxel_size * idx_01);
            u32 val = imageLoad(albedo_map, idx).x;
            v4 S = rgba8_to_v4(val);
            if (S.w > 0.0f)
            {
                occlusion += (1 - occlusion) * smoothstep(0.0f, max_dist, sqrt(march) * SHADOW_STR);
            }

            march += 0.1f;
        }

        C = v4(v3(1 - occlusion) * diffuse_light, 1.0f);
    }
    else // DEBUG
    {
        C = v4(0.03f, 0.02f, 0.1f, 1.0f);
    }
}

)MULTILINE";
