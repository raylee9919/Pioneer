R"MULTILINE(

layout (location = 0) in v3 vP;
layout (location = 2) in v2 vUV;

out v3 fP;
out v2 fUV;

void main()
{
    fP = vP;
    fUV = vUV;

    gl_Position = v4(vP, 1);
}

)MULTILINE"
