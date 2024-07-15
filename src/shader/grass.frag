R"MULTILINE(
smooth in v3 fP;
smooth in v3 fN;
smooth in v2 fUV;
smooth in v4 fC;

out v4 C;

void main()
{
    C = fC;
#if 1
    v3 result_normal = fN;
    if (!gl_FrontFacing)
    {
        result_normal = -result_normal;
    }

    v3 light_color      = v3(1, 1, 1) * 10.0f;
    f32 light_ambient   = 0.9f;
    f32 light_diffuse   = 0.1f;
    v3 light_pos        = v3(0.0f, 10.0f, 0.0f);
    v3 light_sum        = v3(0, 0, 0);

    v3 to_light         = normalize(light_pos - fP);

    // distance falloff
    f32 d = distance(light_pos, fP) * 0.5f;
    light_color /= (d * d);

    // ambient
    v3 ambient_light = light_ambient * light_color;

    // diffuse
    f32 cos_falloff = max(dot(result_normal, to_light), 0.0f);
    v3 diffuse_light = cos_falloff * light_diffuse * light_color;

    light_sum = (ambient_light + diffuse_light);

    C *= v4(light_sum, 1.0f);

#endif

    // sRGB
    if (C.a == 0.0f) 
        discard;
    else
        C.rgb = sqrt(C.rgb);
}

)MULTILINE"
