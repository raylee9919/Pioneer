Member_Definition Members_Of_Entity[] = 
{
    {eMeta_Type_Entity_Type, "type", (u32)&(((Entity *)0)->type)},
    {eMeta_Type_Chunk_Position, "chunk_pos", (u32)&(((Entity *)0)->chunk_pos)},
    {eMeta_Type_v3, "world_translation", (u32)&(((Entity *)0)->world_translation)},
    {eMeta_Type_qt, "world_rotation", (u32)&(((Entity *)0)->world_rotation)},
    {eMeta_Type_v3, "world_scaling", (u32)&(((Entity *)0)->world_scaling)},
    {eMeta_Type_v3, "velocity", (u32)&(((Entity *)0)->velocity)},
    {eMeta_Type_v3, "accel", (u32)&(((Entity *)0)->accel)},
    {eMeta_Type_f32, "u", (u32)&(((Entity *)0)->u)},
    {eMeta_Type_u32, "flags", (u32)&(((Entity *)0)->flags)},
    {eMeta_Type_Animation, "cur_anim", (u32)&(((Entity *)0)->cur_anim)},
    {eMeta_Type_f32, "anim_dt", (u32)&(((Entity *)0)->anim_dt)},
    {eMeta_Type_Entity, "next", (u32)&(((Entity *)0)->next)},
};
Member_Definition Members_Of_Rect2[] = 
{
    {eMeta_Type_v2, "min", (u32)&(((Rect2 *)0)->min)},
    {eMeta_Type_v2, "max", (u32)&(((Rect2 *)0)->max)},
};
Member_Definition Members_Of_Rect3[] = 
{
    {eMeta_Type_v3, "cen", (u32)&(((Rect3 *)0)->cen)},
    {eMeta_Type_v3, "dim", (u32)&(((Rect3 *)0)->dim)},
};
#define META_HANDLE_TYPE_DUMP(member_ptr, next_indent_level) \
    case eMeta_Type_Rect3: { debug_text_line(member->name); debug_dump_struct(array_count(Members_Of_Rect3), Members_Of_Rect3, member_ptr, (next_indent_level)); } break; \
    case eMeta_Type_Rect2: { debug_text_line(member->name); debug_dump_struct(array_count(Members_Of_Rect2), Members_Of_Rect2, member_ptr, (next_indent_level)); } break; \
    case eMeta_Type_Entity: { debug_text_line(member->name); debug_dump_struct(array_count(Members_Of_Entity), Members_Of_Entity, member_ptr, (next_indent_level)); } break; 
