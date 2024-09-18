R"MULTILINE(

#define MAX_BONE_PER_VERTEX         4
#define MAX_BONE_PER_MESH           100

uniform m4x4  world_transform;
uniform m4x4  V;
uniform m4x4  ortho_P;
uniform m4x4  persp_P;
uniform s32   is_skeletal;

layout (location = 0) in v3 vP;

uniform mat4x4                  bone_transforms[MAX_BONE_PER_MESH];
layout (location = 4) in s32    bone_ids[MAX_BONE_PER_VERTEX];
layout (location = 5) in f32    bone_weights[MAX_BONE_PER_VERTEX];

out v3 clip_P;

void main()
{
    // Animation
    m4x4 final_transform;
    if (is_skeletal != 0)
    {
        mat4x4 bone_transform;
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

    v4 result_pos = final_transform * v4(vP, 1.0f);
    gl_Position = ortho_P * V * result_pos;
    clip_P = (ortho_P * V * result_pos).xyz;
}

)MULTILINE"
