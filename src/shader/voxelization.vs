R"MULTILINE(

#define MAX_BONE_PER_VERTEX         4
#define MAX_BONE_PER_MESH           100

uniform m4x4  world_transform;
uniform m4x4  V;
uniform m4x4  P;
uniform m4x4  VP;
uniform s32   is_skeletal;

layout (location = 0) in v3 vP;
layout (location = 1) in v3 vN;
layout (location = 2) in v2 vUV;
layout (location = 3) in v4 vC;

uniform m4x4                    bone_transforms[MAX_BONE_PER_MESH];
layout (location = 4) in s32    bone_ids[MAX_BONE_PER_VERTEX];
layout (location = 5) in f32    bone_weights[MAX_BONE_PER_VERTEX];

out v3 clip_P;
out v3 gN;
out v2 gUV;
out v4 gC;
out v3 world_gP;

void main()
{
    // Animation
    m4x4 final_transform;
    if (is_skeletal != 0)
    {
        m4x4 bone_transform;
        if (bone_ids[0] != -1)
        {
            bone_transform = bone_transforms[bone_ids[0]] * bone_weights[0];
            for (s32 idx = 1;
                 idx < MAX_BONE_PER_VERTEX;
                 ++idx)
            {
                s32 bone_id = bone_ids[idx];
                if (bone_id != -1)
                {
                    bone_transform += bone_transforms[bone_id] * bone_weights[idx];
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            bone_transform = identity();
        }

        final_transform = world_transform * bone_transform;
    }
    else
    {
        final_transform = world_transform;
    }

    //
    //
    //

    v4 world_P = final_transform * v4(vP, 1.0f); // World Coord.
    clip_P = (VP * world_P).xyz;
    world_gP = world_P.xyz;
    gN  = normalize(m3x3(final_transform) * vN);
    gUV = vUV;
    gC  = vC;

    gl_Position = VP * world_P;
}


)MULTILINE"
