/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */



internal void
load_model(Asset_Model **asset_model, const char *file_name,
           Memory_Arena *arena, Read_Entire_File *read_entire_file)
{
    *asset_model = push_struct(arena, Asset_Model);
    Entire_File entire_file = read_entire_file(file_name);
    u8 *at                  = (u8 *)entire_file.contents;
    u8 *end                 = at + entire_file.content_size;

    Asset_Model *model = *asset_model;

    model->mesh_count = *(u32 *)at;
    at += sizeof(model->mesh_count);

    model->meshes = push_array(arena, Asset_Mesh, model->mesh_count);
    Asset_Mesh *mesh = model->meshes;
    size_t tmp_size;
    for (u32 mesh_idx = 0;
         mesh_idx < model->mesh_count;
         ++mesh_idx)
    {
        mesh->vertex_count = *(u32 *)at;
        at += sizeof(u32);

        tmp_size = sizeof(Asset_Vertex) * mesh->vertex_count;
        mesh->vertices = (Asset_Vertex *)push_size(arena, tmp_size);
        memcpy(mesh->vertices, at, tmp_size);
        at += tmp_size;

        mesh->index_count = *(u32 *)at;
        at += sizeof(u32);

        tmp_size = sizeof(u32) * mesh->index_count;
        mesh->indices = (u32 *)push_size(arena, tmp_size);
        memcpy(mesh->indices, at, tmp_size);
        at += tmp_size;

        mesh->material_idx = *(u32 *)at;
        at += sizeof(u32);

        ++mesh;
    }

    // MATERIAL
    model->material_count = *(u32 *)at;
    at += sizeof(u32);

    tmp_size = sizeof(Asset_Material) * model->material_count;
    model->materials = (Asset_Material *)push_size(arena, tmp_size);
    memcpy(model->materials, at, tmp_size);
    at += tmp_size;


    // SKELETAL
    model->bone_count = *(u32 *)at;
    at += sizeof(u32);

    if (model->bone_count)
    {
        model->root_bone_id = *(s32 *)at;
        at += sizeof(s32);

        model->root_transform = *(m4x4 *)at;
        at += sizeof(m4x4);

        tmp_size = sizeof(Asset_Bone) * model->bone_count;
        model->bones = (Asset_Bone *)push_size(arena, tmp_size);
        memcpy(model->bones, at, tmp_size);
        at += tmp_size;
    }

    // ANIMATION
    model->anim_count = *(u32 *)at;
    at += sizeof(u32);

    if (model->anim_count)
    {
        model->anims = push_array(arena, Asset_Animation, model->anim_count);
        for (u32 anim_idx = 0;
             anim_idx < model->anim_count;
             ++anim_idx)
        {
            Asset_Animation *anim = model->anims + anim_idx;
            
            anim->id = *(s32 *)at;
            at += sizeof(s32);

            anim->duration = *(f32 *)at;
            at += sizeof(f32);

            anim->bone_count = *(u32 *)at;
            at += sizeof(u32);

            if (anim->bone_count)
            {
                anim->bones = push_array(arena, Asset_Animation_Bone, anim->bone_count);
            }

            for (u32 bone_idx = 0;
                 bone_idx < anim->bone_count;
                 ++bone_idx)
            {
                Asset_Animation_Bone *anim_bone = anim->bones + bone_idx;

                anim_bone->bone_id = *(s32 *)at;
                at += sizeof(s32);

                anim_bone->translation_count = *(u32 *)at;
                at += sizeof(u32);

                anim_bone->rotation_count = *(u32 *)at;
                at += sizeof(u32);

                anim_bone->scaling_count = *(u32 *)at;
                at += sizeof(u32);

                tmp_size = sizeof(dt_v3_Pair) * anim_bone->translation_count;
                anim_bone->translations = (dt_v3_Pair *)push_size(arena, tmp_size);
                memcpy(anim_bone->translations, at, tmp_size);
                at += tmp_size;

                tmp_size = sizeof(dt_qt_Pair) * anim_bone->rotation_count;
                anim_bone->rotations = (dt_qt_Pair *)push_size(arena, tmp_size);
                memcpy(anim_bone->rotations, at, tmp_size);
                at += tmp_size;

                tmp_size = sizeof(dt_v3_Pair) * anim_bone->scaling_count;
                anim_bone->scalings = (dt_v3_Pair *)push_size(arena, tmp_size);
                memcpy(anim_bone->scalings, at, tmp_size);
                at += tmp_size;
            }
        }


    }

    Assert(at == end);
}

internal void
load_bone_hierarchy(Memory_Arena *arena, Game_Assets *game_assets)
{
    Entire_File entire_file = game_assets->read_entire_file("bones.pack");
    u8 *at                  = (u8 *)entire_file.contents;
    u8 *end                 = at + entire_file.content_size;

    for (u32 id = 0;
         id < MAX_BONE_PER_MESH;
         ++id)
    {
        Asset_Bone_Info *bone = &game_assets->bone_hierarchy.bone_infos[id];

        bone->child_count = *(u32 *)at;
        at += sizeof(u32);

        if (bone->child_count)
        {
            size_t tmp_size = sizeof(s32) * bone->child_count;
            bone->child_ids = (s32 *)push_size(arena, tmp_size);
            memcpy(bone->child_ids, at, tmp_size);
            at += tmp_size;
        }
    }

    Assert(at == end);
}
    


internal void
load_font(Memory_Arena *arena, Read_Entire_File *read_file, Game_Assets *game_assets)
{
    Entire_File read = read_file(ASSET_FILE_NAME);
    u8 *at = (u8 *)read.contents;
    u8 *end = at + read.content_size;

    // parse font header.
    u32 kern_count = ((Asset_Font_Header *)at)->kerning_pair_count;
    game_assets->v_advance = ((Asset_Font_Header *)at)->vertical_advance;
    at += sizeof(Asset_Font_Header);

    // parse kerning pairs.
    for (u32 count = 0; count < kern_count; ++count) 
    {
        Asset_Kerning *asset_kern = (Asset_Kerning *)at;

        Kerning *kern = push_struct(arena, Kerning);
        *kern = {};
        kern->first = asset_kern->first;
        kern->second = asset_kern->second;
        kern->value = asset_kern->value;

        u32 entry_idx = kerning_hash(&game_assets->kern_hashmap, kern->first, kern->second);
        push_kerning(&game_assets->kern_hashmap, kern, entry_idx);
        at += sizeof(Asset_Kerning);
    }

    // parse glyphs.
    if (read.content_size != 0) 
    {
        while (at < end) 
        {
            Asset_Glyph *glyph = (Asset_Glyph *)at;
            Bitmap *bitmap = &glyph->bitmap;
            game_assets->glyphs[glyph->codepoint] = glyph;
            at += sizeof(Asset_Glyph);
            glyph->bitmap.memory = at;
            at += glyph->bitmap.size;
        }
    }
}

#if 1
// IMPORTANT: bitmap->memory starts with lowest scanline of the image.
internal Bitmap *
load_bmp(Memory_Arena *arena, Read_Entire_File *read_file, const char *filename)
{
    Bitmap *result = push_struct(arena, Bitmap);
    *result = {};
    
    Entire_File read = read_file(filename);
    if (read.content_size != 0) 
    {
        BMP_Info_Header *header = (BMP_Info_Header *)read.contents;
        u32 *pixels = (u32 *)((u8 *)read.contents + header->bitmap_offset);

        result->memory  = pixels + header->width * (header->height - 1);
        result->width   = header->width;
        result->height  = header->height;
        result->pitch   = result->width * 4;
        result->size    = result->width * result->height * 4;
        result->handle  = 0;

        Assert(header->compression == 3);

        u32 r_mask = header->r_mask;
        u32 g_mask = header->g_mask;
        u32 b_mask = header->b_mask;
        u32 a_mask = ~(r_mask | g_mask | b_mask);        
        
        Bit_Scan_Result r_scan = find_least_significant_set_bit(r_mask);
        Bit_Scan_Result g_scan = find_least_significant_set_bit(g_mask);
        Bit_Scan_Result b_scan = find_least_significant_set_bit(b_mask);
        Bit_Scan_Result a_scan = find_least_significant_set_bit(a_mask);
        
        Assert(r_scan.found);
        Assert(g_scan.found);   
        Assert(b_scan.found);
        Assert(a_scan.found);

        s32 r_shift = (s32)r_scan.index;
        s32 g_shift = (s32)g_scan.index;
        s32 b_shift = (s32)b_scan.index;
        s32 a_shift = (s32)a_scan.index;

        f32 inv_255f = 1.0f / 255.0f;
        
        u32 *at = pixels;
        for(s32 y = 0;
            y < header->height;
            ++y)
        {
            for(s32 x = 0;
                x < header->width;
                ++x)
            {
                u32 c = *at;

                f32 r = (f32)((c & r_mask) >> r_shift);
                f32 g = (f32)((c & g_mask) >> g_shift);
                f32 b = (f32)((c & b_mask) >> b_shift);
                f32 a = (f32)((c & a_mask) >> a_shift);

                f32 ra = a * inv_255f;
#if 1
                r *= ra;
                g *= ra;
                b *= ra;
#endif

                *at++ = (((u32)(a + 0.5f) << 24) |
                         ((u32)(r + 0.5f) << 16) |
                         ((u32)(g + 0.5f) <<  8) |
                         ((u32)(b + 0.5f) <<  0));
            }
        }
        result->memory = pixels;
    }


    return result;
}
#endif


#if 0
PLATFORM_WORK_QUEUE_CALLBACK(load_asset_work)
{
    Load_Asset_Work_Data *workData = (Load_Asset_Work_Data *)data;
    workData->game_assets->bitmaps[workData->assetID] = load_bmp(workData->assetArena, workData->game_assets->read_entire_file, workData->fileName);
    workData->game_assets->bitmapStates[workData->assetID] = Asset_State_Loaded;
    end_work_memory(workData->workSlot);
}
#endif

#if 0
internal Bitmap *
GetBitmap(Transient_State *trans_state, Asset_ID assetID,
          PlatformWorkQueue *queue, Platform_API *platform)
{
    Bitmap *result = trans_state->game_assets.bitmaps[assetID];

    if (!result) 
    {
        if (atomic_compare_exchange_u32((u32 *)&trans_state->game_assets.bitmapStates[assetID],
                                        Asset_State_Queued, Asset_State_Unloaded)) 
        {
            WorkMemory_Arena *workSlot = begin_work_memory(trans_state);
            if (workSlot) 
            {
                Load_Asset_Work_Data *workData = push_struct(&workSlot->memoryArena, Load_Asset_Work_Data);
                workData->game_assets = &trans_state->game_assets;
                workData->assetArena = &trans_state->assetArena;
                workData->assetID = assetID;
                workData->workSlot = workSlot;

                switch(assetID) 
                {
                    case GAI_Tree: 
                    {
                        workData->fileName = "tree2_teal.bmp";
                    } break;

                    case GAI_Particle: 
                    {
                        workData->fileName = "white_particle.bmp";
                    } break;

                    INVALID_DEFAULT_CASE
                }
#if 1 // multi-thread
                platform->platform_add_entry(queue, load_asset_work, workData);
                return 0; // todo: no bmp...?
#else // single-thread
                load_asset_work(queue, workData);
                return 0; // todo: no bmp...?
#endif
            } 
            else 
            {
                return result;
            }
        } 
        else 
        {
            return result;
        }
    } 
    else 
    {
        return result;
    }
}
#endif

