/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

internal m4x4
eval_node(Animation *anim, f32 dt, Node *node)
{
    m4x4 result = node->transform;

    for (u32 node_idx = 0;
         node_idx < anim->node_count;
         ++node_idx)
    {
        Animation_Node *anim_node = (anim->nodes + node_idx);
        if (node->id == anim_node->id)
        {
            // lerp translation.
            v3 lerped_translation = (anim_node->translations + (anim_node->translation_count - 1))->vec;
            for (u32 translation_idx = 0;
                 translation_idx < anim_node->translation_count;
                 ++translation_idx)
            {
                dt_v3_Pair *hi_key = anim_node->translations + translation_idx;
                if (hi_key->dt > dt)
                {
                    dt_v3_Pair *lo_key = (hi_key - 1);
                    f32 t = (dt - lo_key->dt) / (hi_key->dt - lo_key->dt);
                    lerped_translation = lerp(lo_key->vec, t, hi_key->vec);
                    break;
                }
                else if (hi_key->dt == dt)
                {
                    lerped_translation = hi_key->vec;
                    break;
                }
            }

            // slerp rotation.
            qt slerped_rotation = (anim_node->rotations + (anim_node->rotation_count - 1))->q;
            for (u32 rotation_idx = 0;
                 rotation_idx < anim_node->rotation_count;
                 ++rotation_idx)
            {
                dt_qt_Pair *hi_key = anim_node->rotations + rotation_idx;
                if (hi_key->dt > dt)
                {
                    dt_qt_Pair *lo_key = (hi_key - 1);
                    f32 t = (dt - lo_key->dt) / (hi_key->dt - lo_key->dt);
                    slerped_rotation = slerp(lo_key->q, t, hi_key->q);
                    break;
                }
                else if (hi_key->dt == dt)
                {
                    slerped_rotation = hi_key->q;
                    break;
                }
            }

            // lerp scaling.
            v3 lerped_scaling = (anim_node->scalings + (anim_node->scaling_count - 1))->vec;
            for (u32 scaling_idx = 0;
                 scaling_idx < anim_node->scaling_count;
                 ++scaling_idx)
            {
                dt_v3_Pair *hi_key = anim_node->scalings + scaling_idx;
                if (hi_key->dt > dt)
                {
                    dt_v3_Pair *lo_key = (hi_key - 1);
                    f32 t = (dt - lo_key->dt) / (hi_key->dt - lo_key->dt);
                    lerped_scaling = lerp(lo_key->vec, t, hi_key->vec);
                    break;
                }
                else if (hi_key->dt == dt)
                {
                    lerped_scaling = hi_key->vec;
                    break;
                }
            }

            result = trs_to_transform(lerped_translation, slerped_rotation, lerped_scaling);
            break;
        }
    }

    return result;
}

struct Eval_Stack_Frame
{
    s32 node_id;
    b32 global_transform_done;
    u32 next_child_idx;

    m4x4 global_transform;
};
struct Eval_Stack
{
    Eval_Stack_Frame frames[256];
    u32 top;
};
internal void
eval(Model *model, Animation *anim, f32 dt, m4x4 *final_transforms)
{
    Eval_Stack stack = {};

    Eval_Stack_Frame *frame = stack.frames;
    for (;;)
    {
        Node *node = model->nodes + frame->node_id;

        if (frame->next_child_idx == node->child_count)
        {
            if (stack.top == 0) 
                break;
            else 
            {
                stack.frames[stack.top--] = {};
            }
        }
        else
        {
            if (!frame->global_transform_done)
            {
                m4x4 node_transform = eval_node(anim, dt, node);
                m4x4 parent_transform = (stack.top != 0) ? stack.frames[stack.top - 1].global_transform : identity();
                m4x4 global_transform = parent_transform * node_transform;
                m4x4 final_transform = global_transform * node->offset;

                frame->global_transform = global_transform;
                final_transforms[node->id] = final_transform;

                frame->global_transform_done = true;
            }

            ++stack.top;
            stack.frames[stack.top].node_id = node->child_ids[frame->next_child_idx++];
        }

        frame = stack.frames + stack.top;
    }
}
