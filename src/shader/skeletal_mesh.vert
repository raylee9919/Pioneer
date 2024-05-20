R"MULTILINE(

#define MAX_BONE_PER_VERTEX         4
#define MAX_BONE_PER_MESH           100

uniform mat4x4  world_transform;
uniform mat4x4  mvp;
uniform s32     is_skeletal;

layout (location = 0) in vec3 vP;
layout (location = 1) in vec3 vN;
layout (location = 2) in vec2 vUV;
layout (location = 3) in vec4 vC;

uniform mat4x4                  bone_transforms[MAX_BONE_PER_MESH];
layout (location = 4) in s32    bone_ids[MAX_BONE_PER_VERTEX];
layout (location = 5) in f32    bone_weights[MAX_BONE_PER_VERTEX];

smooth out vec3 fP;
smooth out vec3 fN;
smooth out vec2 fUV;
smooth out vec4 fC;


void main()
{
    mat4x4 final_transform;
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

    vec4 result_pos = final_transform * vec4(vP, 1.0f);
    fP  = result_pos.xyz;
    fN  = normalize(mat3x3(final_transform) * vN);
    fUV = vUV;
    fC  = vC;

    gl_Position = mvp * result_pos;
}

)MULTILINE"
