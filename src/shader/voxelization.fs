R"===(

uniform f32 time;
uniform sampler2D texture_sampler;
uniform layout(binding = 0, offset = 0) atomic_uint fragment_counter;

uniform layout(binding = 0, rgb10_a2ui) uimageBuffer flist_P;
uniform layout(binding = 1, rgba8) imageBuffer flist_diffuse;


uniform v3 ambient;
uniform v3 diffuse;
uniform v3 specular;

uniform v3 DEBUG_light_P;
uniform v3 DEBUG_light_color;
uniform f32 DEBUG_light_strength;

uniform u32 octree_level;
uniform u32 octree_resolution;

in v3 clip_fP;
in v3 world_fP;
in v3 fN;
in v2 fUV;
in v4 fC;

void main()
{
    v3 coord_01 = 0.5f * clip_fP + v3(0.5f);
    f32 voxel_size = f32(octree_resolution);
    iv3 coord = iv3(coord_01 * voxel_size);

    v3 tmp = v3(0, 2, 0);
    f32 cos_falloff = max(dot(fN, normalize(DEBUG_light_P - world_fP)), 0.0f);
    f32 attenuation = light_attenuation(DEBUG_light_P, world_fP);

    // Increment fragment count.
    s32 next_empty = s32(atomicCounterIncrement(fragment_counter));


    //
    // Assign voxel coordinate to fragment list
    //
    imageStore(flist_P, next_empty, uv4(coord, 0));



    //
    // Diffuse Light
    //
    imageStore(flist_diffuse, next_empty, v4(1.0f));



#if 0
    //
    // DIFFUSE
    //
    {
        v3 diffuse = DEBUG_light_color * DEBUG_light_strength * diffuse * cos_falloff * attenuation;
        v4 val = v4(diffuse, 1.0f);
        u32 new_val = v4_to_rgba8(val);
        u32 prev = 0;
        u32 cur;
        u32 count = 0;

        // @TODO: this is taxy.
        while((cur = imageAtomicCompSwap(albedo_map, coord, prev, new_val)) != prev &&
              //false)
              count < 10)
        {
            prev = cur;
            v4 rval = rgba8_to_v4(cur);
            rval.rgb = (rval.rgb * rval.a);
            v4 cur_f = rval + val;
            cur_f.rgb /= cur_f.a;
            new_val = v4_to_rgba8(cur_f);

            ++count;
        }
    }
#endif











#if 0
    //
    // NORMAL
    //
    {
        v4 val = v4(fN, 1.0f);
        u32 new_val = v4_to_rgba8(val);
        u32 prev = 0;
        u32 cur;
        u32 count = 0;

        while((cur = imageAtomicCompSwap(normal_map, idx, prev, new_val)) != prev &&
              count < 255)
        {
            prev = cur;
            v4 rval = rgba8_to_v4(cur);
            rval.rgb = (rval.rgb * rval.a);
            v4 cur_f = rval + val;
            cur_f.rgb /= cur_f.a;
            new_val = v4_to_rgba8(cur_f);

            ++count;
        }
    }
#endif

}

)===";
