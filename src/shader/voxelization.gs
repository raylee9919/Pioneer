R"MULTILINE(

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in v3 clip_P[];
in v3 gN[];
in v2 gUV[];
in v4 gC[];
in v3 world_gP[];

out v3 clip_fP;
out v3 fN;
out v2 fUV;
out v4 fC;
out v3 world_fP;

void main() 
{
    v3 e1 = clip_P[0] - clip_P[2];
    v3 e2 = clip_P[1] - clip_P[2];
    v3 tN = normalize(abs(cross(e1, e2)));
    f32 dominant_axis = max(max(tN.x, tN.y), tN.z);

	for (u32 i = 0;
         i < 3;
         i++) 
    {
        clip_fP = clip_P[i];
        fN = gN[i];
        fUV = gUV[i];
        fC = gC[i];
        world_fP = world_gP[i];

        if (dominant_axis == tN.x)
        {
            gl_Position = v4(clip_fP.zyx, 1.0f);
        }
        else if (dominant_axis == tN.y)
        {
            gl_Position = v4(clip_fP.xzy, 1.0f);
        }
        else
        {
            gl_Position = v4(clip_fP, 1.0f);
        }


		EmitVertex();
	}

	EndPrimitive();
}


)MULTILINE"
