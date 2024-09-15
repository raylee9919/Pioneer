R"MULTILINE(

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in v3 gP[];
in v3 gN[];

out v3 fP;
out v3 fN;

void main() 
{
    v3 tN = abs(cross(gP[0] - gP[1], gP[0] - gP[2]));

	f32 dominant_axis = max(max(tN.x, tN.y), tN.z);
	
	for (u32 i = 0;
         i < 3;
         i++) 
    {
        fP = gP[i]; // Camera Coord.
        fN = gN[i]; // Camera Coord.

		if (dominant_axis == tN.x)
        {
            gl_Position = v4(fP.zyx, 1); // Camera Coord.
        }
		else if (dominant_axis == tN.y)
        {
            gl_Position = v4(fP.xzy, 1);
        }
		else if (dominant_axis == tN.z)
        {
            gl_Position = v4(fP.xyz, 1);
        }

		EmitVertex();
	}

	EndPrimitive();
}


)MULTILINE"
