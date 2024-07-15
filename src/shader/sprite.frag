R"MULTILINE(

uniform v4 color;
uniform sampler2D texture_sample;

in v2   fUV;
out v4  C;

void main()
{
    // sRGB
    vec4 const_color = color;
    const_color.r *= const_color.r;
    const_color.g *= const_color.g;
    const_color.b *= const_color.b;

    // sRGB
    vec4 texture_color = texture(texture_sample, fUV);
    texture_color.r *= texture_color.r;
    texture_color.g *= texture_color.g;
    texture_color.b *= texture_color.b;

    C = const_color * texture_color;

    // sRGB
    if (C.a == 0.0f) 
        discard;
    else
        C.rgb = sqrt(C.rgb);
}

)MULTILINE"
