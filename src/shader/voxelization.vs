R"MULTILINE(

layout (location = 0) in v3 vP;
layout (location = 1) in v3 vN;

out v3 gP;
out v3 gN;

void main()
{
    gl_Position = mvp * v4(gP, 1.0f);
}

)MULTILINE"
