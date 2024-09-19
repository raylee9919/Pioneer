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


void main()
{
#if 0
#define POINT_LIGHT 1
    C = texture(texture_sampler, fUV) * fC;

    //v3 light_pos        = v3(0.0f, 2.0f, 0.0f);
    v3 light_color      = v3(1.0f, 1.0f, 1.0f) * 7.0f;
    f32 light_diffuse   = 0.5f;
    f32 light_specular  = 1.0f;
    v3 light_sum;
#if POINT_LIGHT
    v3 to_light         = normalize(light_pos - fP);
#else
    v3 to_light         = normalize(vec3(0, 1, 0));
#endif
    v3 from_cam         = normalize(fP - cam_pos);

    // ambient
#if 0
    v3 ambient_light = color_ambient * light_color;
#else
    v3 ambient_light = v3(0, 0, 0);
#endif

#if POINT_LIGHT
    // distance falloff
    f32 d = distance(light_pos, fP);
    light_color /= (d * d);
    f32 attenuation = light_attenuation(fP, light_pos);
#else
    f32 attenuation = 1.0f;
#endif

    // diffuse
    f32 cos_falloff = max(dot(fN, to_light), 0.0f);
    v3 diffuse_light = cos_falloff * color_diffuse * light_diffuse * light_color * attenuation;

    // specular
    v3 ref = normalize(from_cam - 2 * dot(fN, from_cam) * fN);
    f32 cos_ref = max(dot(ref, to_light), 0.0f);
    cos_ref *= cos_ref;
    v3 specular_light = cos_ref * color_specular * light_specular * light_color;

    light_sum = (ambient_light + diffuse_light + specular_light);

    C *= v4(light_sum, 1.0f);


    // sRGB
    if (C.a == 0.0f) 
        discard;
    else
        C.rgb = sqrt(C.rgb);

#else
    v3 light_P = v3(0, 2, 0);
    v3 voxel_size = imageSize(albedo_map);
    v3 idx_01 = (voxel_VP * v4(fP, 1)).xyz * 0.5f + v3(0.5f);
    iv3 idx = iv3(voxel_size * idx_01);
    u32 val = imageLoad(albedo_map, idx).x;
    C = rgba8_to_v4(val);

#endif
}

)MULTILINE"
