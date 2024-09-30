R"MULTILINE(

#define MAX_BONE_PER_VERTEX         4
#define MAX_BONE_PER_MESH           100

uniform m4x4  world_transform;
uniform m4x4  V;
uniform m4x4  persp_P;
uniform m4x4  ortho_P;
uniform s32   is_skeletal;

layout (location = 0) in v3 vP;
layout (location = 1) in v3 vN;

uniform m4x4                    bone_transforms[MAX_BONE_PER_MESH];
layout (location = 4) in s32    bone_ids[MAX_BONE_PER_VERTEX];
layout (location = 5) in f32    bone_weights[MAX_BONE_PER_VERTEX];

out v4 persp_clip_P;
out v3 ortho_clip_P;
out v4 world_fP;

void main()
{
    // Animation
    m4x4 M;
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

        M = world_transform * bone_transform;
    }
    else
    {
        M = world_transform;
    }

    v4 world_P = M * v4(vP, 1.0f);

    //
    //
    //
    world_fP = world_P;

    v4 tmp = ortho_P * world_P;
    ortho_clip_P = (tmp.xyz / tmp.w);
    persp_clip_P = persp_P * V * world_P;
    gl_Position = tmp;
}

)MULTILINE"
