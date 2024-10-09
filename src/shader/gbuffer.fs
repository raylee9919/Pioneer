R"(

uniform sampler2D diffuse_texture;
uniform v3 ambient;
uniform v3 diffuse;
uniform v3 specular;

in v3 fP;
in v3 fN;
in v2 fUV;
in v4 fC;

layout (location = 0) out v4 gP;
layout (location = 1) out v3 gN;
layout (location = 2) out v4 gC;

void main()
{
    gP = v4(fP, 1);
    gN = normalize(fN);
    gC = (texture(diffuse_texture, fUV) * fC * v4(diffuse, 1));
    gC.rgb = gC.rgb*gC.rgb;
}

)";
