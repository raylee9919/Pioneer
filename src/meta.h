/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */

enum Meta_Type
{
    eMeta_Type_Entity_Type,
    eMeta_Type_Chunk_Position,
    eMeta_Type_v2,
    eMeta_Type_v3,
    eMeta_Type_Rect2,
    eMeta_Type_Rect3,
    eMeta_Type_qt,
    eMeta_Type_f32,
    eMeta_Type_u32,
    eMeta_Type_Asset_Animation,
    eMeta_Type_Entity,
};

struct Member_Definition
{
    Meta_Type type;
    char *name;
    u32 offset;
};
