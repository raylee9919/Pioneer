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
uniform u32 write;

in v3 clip_fP;
in v3 world_fP;
in v3 fN;
in v2 fUV;
in v4 fC;

flat in u32 axis;
flat in v4 fAABB;

void main()
{
    if (clip_fP.x >= fAABB.x &&
        clip_fP.y >= fAABB.y &&
        clip_fP.x <= fAABB.z &&
        clip_fP.y <= fAABB.w &&
        is_in_clip_space(clip_fP))
    {
        v3 coord_01 = 0.5f * clip_fP + v3(0.5f);
        v3 coord = f32(octree_resolution) * coord_01;
        uv3 ucoord = uv3(coord);

        // DEBUG light
        f32 cos_falloff = max(dot(fN, normalize(DEBUG_light_P - world_fP)), 0.0f);
        f32 attenuation = light_attenuation(DEBUG_light_P, world_fP);
    
        // Increment fragment count.
        s32 next_empty = s32(atomicCounterIncrement(fragment_counter));

        if (write == 1)
        {
            //v4 val = v4(DEBUG_light_color * (fC.xyz * diffuse) * cos_falloff * attenuation, 1);
            v4 val = v4(fC.xyz * diffuse, 1);
            //v4 val = v4(1, 0, 1, 1);

            imageStore(flist_P, next_empty, uv4(ucoord, 0));
            imageStore(flist_diffuse, next_empty, val);
        }
    
    
    
    
    
    
    
    }
}

)===";
