R"MULTILINE(

uniform sampler2D   texture_sampler;
uniform v3        cam_pos;

// material for mesh.
uniform v3 color_ambient;
uniform v3 color_diffuse;
uniform v3 color_specular;

smooth in v3 fP;
smooth in v3 fN;
smooth in v2 fUV;
smooth in v4 fC;

out v4 C;

void main()
{
    C = texture(texture_sampler, fUV) * fC;

    v3 light_color        = v3(1.0f, 1.0f, 1.0f);
    v3 light_pos          = v3(0.0f, 0.0f, 2.0f);
    v3 light_sum          = v3(0.0f, 0.0f, 0.0f);
    v3 to_light           = normalize(light_pos - fP);
    v3 from_cam           = normalize(fP - cam_pos);

    // ambient
    v3 ambient_light = color_ambient * light_color;

    // distance falloff
    f32 d = distance(light_pos, fP);
    light_color /= (d * d);

    // diffuse
    f32 cos_falloff = clamp(dot(fN, to_light), 0.0f, 1.0f);
    v3 diffuse_light = cos_falloff * color_diffuse *  light_color;

    // specular
    v3 ref = normalize(from_cam - 2 * dot(fN, from_cam) * fN);
    f32 cos_ref = clamp(dot(ref, to_light), 0.0f, 1.0f);
    v3 specular_light = cos_ref * color_specular * light_color;

    light_sum += ambient_light + diffuse_light + specular_light;

    C.xyz *= light_sum;

    if (C.a == 0.0f) 
    {
        discard;
    }
}

)MULTILINE"
