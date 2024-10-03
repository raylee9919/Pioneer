R"MULTILINE(

in v3 fP;
in v2 fUV;

out v4 C;

uniform u32 octree_level;
uniform u32 octree_resolution;

uniform m4x4  voxel_P;

uniform v3 DEBUG_light_P;
uniform v3 DEBUG_light_color;
uniform f32 DEBUG_light_strength;

uniform layout (binding = 1) sampler2D   gP;
uniform layout (binding = 2) sampler2D   gN;
uniform layout (binding = 3) sampler2D   gC;

uniform layout(binding = 0, r32ui) uimageBuffer octree_nodes;
uniform layout(binding = 1, r32ui) uimageBuffer octree_diffuse;

void main()
{
    // Read from G-Buffer
    v4 fP_read = texture(gP, fUV);
    v3 fP = fP_read.xyz;
    v3 fN = normalize(texture(gN, fUV).xyz); // we normalized on previous pass... but for safety.
    v3 fC = texture(gC, fUV).rgb;

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
        v3 diffuse_light = cos_falloff * fC * DEBUG_light_strength * DEBUG_light_color * attenuation;




    
        #define SHADOW_STR 1.0f
        f32 occlusion = 0.0f;
        f32 march = 0.001f;
        f32 max_dist = distance(DEBUG_light_P, fP);

        while (march < max_dist &&
               occlusion < 1.0f)
        {
            v3 cen = fP + to_light * march;

            v3 idx_01 = (voxel_P * v4(cen, 1)).xyz * 0.5f + v3(0.5f);
            uv3 ucoord = uv3(idx_01 * octree_resolution);

            u32 idx = 0;
            u32 node = 0;

            for (u32 level = 0;
                 level <= octree_level;
                 ++level)
            {
                u32 s = (octree_level - level);
                uv3 sub = ucoord;
                sub >>= s;
                u32 offset = sub.x + (sub.y << 1) + (sub.z << 2);
                sub <<= s;
                ucoord -= sub;
                idx = node + offset;

                node = (imageLoad(octree_nodes, s32(idx)).r & 0x7fffffff);
            }

            u32 val = imageLoad(octree_diffuse, s32(idx)).r;
            v4 S = rgba8_to_v4(val);
            if (S.a > 0)
            {
                occlusion += (1 - occlusion) * smoothstep(0.0f, max_dist, sqrt(march) * SHADOW_STR);
            }

            march += 0.01f;
        }

        C = v4(v3(1 - occlusion) * diffuse_light, 1.0f);
    }
    else // DEBUG
    {
        C = v4(0.03f, 0.02f, 0.1f, 1.0f);
    }
}

)MULTILINE";
