R"MULTILINE(

uniform v4 color;
uniform sampler2D texture_sample;

in v2   fUV;
out v4  C;

void main()
{
    C = color * texture(texture_sample, fUV);
    if (C.a == 0.0f) 
    {
        discard;
    }
}

)MULTILINE"
