R"MULTILINE(

uniform sampler2D   texture_sampler;
uniform v3          cam_pos;

// material for mesh.
uniform v3 color_ambient;
uniform v3 color_diffuse;
uniform v3 color_specular;
uniform v3 light_pos;

smooth in v3 fP;
smooth in v3 fN;
smooth in v2 fUV;
smooth in v4 fC;

out v4 C;

void main()
{
#define POINT_LIGHT 1
    C = texture(texture_sampler, fUV) * fC;

    //v3 light_pos        = v3(0.0f, 2.0f, 0.0f);
    v3 light_color      = v3(1.0f);
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
#endif

    // diffuse
    f32 cos_falloff = max(dot(fN, to_light), 0.0f);
    v3 diffuse_light = cos_falloff * color_diffuse * light_diffuse * light_color;

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
}

)MULTILINE"
