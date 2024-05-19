R"MULTILINE(

uniform m4x4 mvp;

layout (location = 0) in v3 vP;
layout (location = 2) in v2 vUV;

smooth out v2 fUV;

void main()
{
    fUV = vUV;
    gl_Position = mvp * v4(vP, 1.0f);
}

)MULTILINE"
