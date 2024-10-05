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

        C = v4(diffuse_light, 1.0f);
    }
    else // DEBUG
    {
        C = v4(0.03f, 0.02f, 0.1f, 1.0f);
    }
}

)MULTILINE";
