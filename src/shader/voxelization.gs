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

flat out u32 axis;
flat out v4 fAABB;

uniform u32 octree_resolution;

void main() 
{
    //
    // 1. Dominant Axis
    //
    v3 P[3] = {
        clip_P[0],
        clip_P[1],
        clip_P[2]
    };

    // Find dominant axis.
    v3 E1 = P[0] - P[2];
    v3 E2 = P[1] - P[0];
    v3 tN = abs(cross(E1, E2));
    f32 dominant_axis = max(max(tN.x, tN.y), tN.z);

    // Tilt triangle.
    for (u32 i = 0; i < 3; ++i)
    {
        if (dominant_axis == tN.x)
        {
            axis = 0;
            P[i] = P[i].zyx;
        }
        else if (dominant_axis == tN.y)
        {
            axis = 1;
            P[i] = P[i].xzy;
        }
        else
        {
            axis = 2;
            P[i] = P[i].xyz;
        }
    }

    //
    // 2. Conservative Rasterization
    //
    f32 hp = 1.0f / f32(octree_resolution);
    f32 hd = hp * 1.41421356237f;

    v4 AABB;
    AABB.xy = min(min(clip_P[0].xy, clip_P[1].xy), clip_P[2].xy) - v2(hp);
    AABB.zw = max(max(clip_P[0].xy, clip_P[1].xy), clip_P[2].xy) + v2(hp);
    fAABB = AABB;

    v3 E[3] = v3[3](v3(P[2].xy - P[1].xy, 0),
                    v3(P[0].xy - P[2].xy, 0),
                    v3(P[1].xy - P[0].xy, 0));
    v3 N[3] = v3[3](normalize(cross(E[0], v3(0, 0, 1))),
                    normalize(cross(E[1], v3(0, 0, 1))),
                    normalize(cross(E[2], v3(0, 0, 1))));
    f32 S = (P[1].x - P[0].x) * (P[1].y + P[0].y) + 
            (P[2].x - P[1].x) * (P[2].y + P[1].y) +
            (P[0].x - P[2].x) * (P[0].y + P[2].y);
    // @TEMPORARY: This is temporary solution for dilating triangle.
    for (u32 i = 0; i < 3; ++i)
    {
        P[i].xy += (((S < 0) ? -1 : 1) * N[i].xy * hd);
    }

    //
    // 3. Emit
    //
	for (u32 i = 0;
         i < 3;
         i++) 
    {
        fN       = gN[i];
        fUV      = gUV[i];
        fC       = gC[i];
        world_fP = world_gP[i];


        if (dominant_axis == tN.x)
        {
            clip_fP = P[i].zyx;
        }
        else if (dominant_axis == tN.y)
        {
            clip_fP = P[i].xzy;
        }
        else
        {
            clip_fP = P[i].xyz;
        }
        gl_Position = v4(P[i], 1); // @IMPORTANT


		EmitVertex();
	}

	EndPrimitive();
}


)MULTILINE";
