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
    v3 P[3] = {
        clip_P[0],
        clip_P[1],
        clip_P[2]
    };

    // Find dominant axis.
    v3 E1 = P[0] - P[2];
    v3 E2 = P[1] - P[2];
    v3 tN = abs(cross(E1, E2));
    f32 dominant_axis = max(max(tN.x, tN.y), tN.z);

    // Tilt triangle.
    for (u32 i = 0; i < 3; ++i)
    {
        if (dominant_axis == tN.x)
        {
            P[i] = P[i].zyx;
        }
        else if (dominant_axis == tN.y)
        {
            P[i] = P[i].xzy;
        }
    }

    // Conservative rasterization.
#if 0
    u32 octree_level = 10;
    u32 res = (1 << octree_level);
    f32 half_pixel = 1.0f / res;
	f32 pl = 1.4142135637309;

    v3 e0 = v3( P[1].xy - P[0].xy, 0 );
	v3 e1 = v3( P[2].xy - P[1].xy, 0 );
	v3 e2 = v3( P[0].xy - P[2].xy, 0 );
	v3 n0 = normalize(cross(e0, v3(0,0,-1)));
	v3 n1 = normalize(cross(e1, v3(0,0,-1)));
	v3 n2 = normalize(cross(e2, v3(0,0,-1)));

	P[0] += (n0 * 2.0f*half_pixel * pl);
	P[1] += (n1 * 2.0f*half_pixel * pl);
	P[2] += (n2 * 2.0f*half_pixel * pl);
#endif

    // Emit
	for (u32 i = 0;
         i < 3;
         i++) 
    {
        clip_fP  = P[i];
        fN       = gN[i];
        fUV      = gUV[i];
        fC       = gC[i];
        world_fP = world_gP[i];

        gl_Position = v4(clip_fP, 1.0f);

		EmitVertex();
	}

	EndPrimitive();
}


)MULTILINE";
