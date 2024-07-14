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
    case eMeta_Type_Rect2: { debug_text_line(member->name); debug_dump_struct(array_count(Members_Of_Rect2), Members_Of_Rect2, member_ptr, (next_indent_level)); } break; 
