R"MULTILINE(

in v3 fP;
in v2 fUV;

out v4 C;

uniform u32 octree_level;
uniform u32 octree_resolution;

uniform m4x4  voxel_P;

uniform v3 cam_P;

uniform v3 DEBUG_light_P;
uniform v3 DEBUG_light_color;
uniform f32 DEBUG_light_strength;

uniform f32 voxel_in_meter;

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

        f32 march     = SQRT_3 * voxel_in_meter + 0.1f;
        f32 max_dist  = min(distance(fP, DEBUG_light_P), 10.0f);
        f32 occlusion = 0.0f;

        f32 march_step = voxel_in_meter * 0.5f;
        f32 marched_dist = 0.0f;
        b32 is_in = false;

        while (march < max_dist &&
               occlusion < 1.0f)
        {
            v3 cen = fP + to_light * march;
            v3 clip_P = (voxel_P * v4(cen, 1)).xyz;
            v3 coord_01 = clip_P * 0.5f + 0.5f;
            uv3 ucoord = uv3(coord_01 * octree_resolution);

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

                if (node == 0)
                {
                    break;
                }
            }

            v4 val = rgba8_to_v4(imageLoad(octree_diffuse, s32(idx)).r);
            if (val.a > 0)
            {
                marched_dist += march_step;
                is_in = true;
            }
            else if (is_in)
            {
                f32 k = 0.25f;
                occlusion += ((marched_dist*marched_dist) / (k*k));
                marched_dist = 0.0f;
                is_in = false;
            }

            march += march_step;
        }

        //
        // Direct Light
        //
        // Distance Falloff
        f32 attenuation = light_attenuation(fP, DEBUG_light_P);
    
        // Diffuse
        f32 cos_falloff = max(dot(fN, to_light), 0.0f);
        v3 diffuse_light = cos_falloff * fC * DEBUG_light_strength * DEBUG_light_color * attenuation;

        //diffuse_light = v3(1);
        v3 result = diffuse_light * (1 - occlusion);
        C = v4(sqrt(result), 1);
    }
    else // DEBUG
    {
        C = v4(0.03f, 0.02f, 0.1f, 1.0f);
    }
}

)MULTILINE";
