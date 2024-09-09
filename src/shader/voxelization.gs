R"MULTILINE(

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in v3 gP[];
in v3 gN[];

out v3 fP;
out v3 fN;

void main()
{
    v3 face_normal = abs(cross(gP[1] - gP[0], gP[2] - gP[0]));
    f32 dominant_axis = max(face_normal.x, face_normal.y, face_normal.z);

    for (u32 i = 0;
        i < 3;
        ++i)
    {
        fP = gP[i];
        fN = gN[1];

        if (dominant_axis == face_normal.x)
        {
            gl_Position = v4(fP.zyx, 1.0f);
        }
        else if (dominant_axis == face_normal.y)
        {
            gl_Position = v4(fP.xzy, 1.0f);
        }
        else
        {
            gl_Position = v4(fP.xyz, 1.0f);
        }

        EmitVertex();
    }

    EndPrimitive();
}


)MULTILINE"
