R"MULTILINE(

uniform f32 time;
uniform sampler2D texture_sampler;
layout(binding = 0, rgba8) uniform volatile coherent image3D voxel_map;
layout(binding = 1, r32ui) uniform volatile coherent uimage3D albedo_map;
layout(binding = 2, r32ui) uniform volatile coherent uimage3D normal_map;

uniform v3 ambient;
uniform v3 diffuse;
uniform v3 specular;

in v3 clip_fP;
in v3 world_fP;
in v3 fN;
in v2 fUV;
in v4 fC;

void main()
{
    v3 idx_01 = 0.5f * clip_fP + v3(0.5f);
    iv3 voxel_size = imageSize(albedo_map);
    iv3 idx = iv3(idx_01 * voxel_size);

    v3 light_color = v3(1, 1, 1);
    f32 light_strength = 1.0f;
    v3 light_P = v3(0, 2, 0);
    f32 cos_falloff = max(dot(fN, normalize(light_P - world_fP)), 0.0f);
    f32 attenuation = light_attenuation(light_P, world_fP);

    //
    // DIFFUSE
    //
    {
        v3 diffuse = light_color * light_strength * diffuse * cos_falloff * attenuation;
        v4 val = v4(diffuse, 1.0f);
        u32 new_val = v4_to_rgba8(val);
        u32 prev = 0;
        u32 cur;
        u32 count = 0;

        // @TODO: this is taxy.
        while((cur = imageAtomicCompSwap(albedo_map, idx, prev, new_val)) != prev &&
              //false)
              count < 256)
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

)MULTILINE"
