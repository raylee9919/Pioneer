/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */



#define READ(to, type)\
    to = *(type *)at; \
    at += sizeof(to);
#define READ_COUNT(to, type, count) \
    to = push_array(arena, type, count); \
    copy(to, at, sizeof(type)*count); \
    at += (sizeof(type)*count);

//
// In order to achieve animation hot-reloading, we need to pass the pointer of
// the asset, not returning it.
//
internal void
load_model(Model *model, char *file_name, Memory_Arena *arena, Read_Entire_File *read_entire_file)
{
    Assert(model);

    Entire_File entire_file = read_entire_file(file_name);
    Assert(entire_file.content_size);
    u8 *at  = (u8 *)entire_file.contents;
    u8 *end = at + entire_file.content_size;

    READ(model->mesh_count, u32);

    model->meshes = push_array(arena, Mesh, model->mesh_count);
    for (u32 mesh_idx = 0;
         mesh_idx < model->mesh_count;
         ++mesh_idx)
    {
        Mesh *mesh = model->meshes + mesh_idx;

        READ(mesh->vertex_count, u32);
        mesh->vertices = push_array(arena, Vertex, mesh->vertex_count);
        for (u32 vertex_idx = 0;
             vertex_idx < mesh->vertex_count;
             ++vertex_idx)
        {
            Vertex *vertex = mesh->vertices + vertex_idx;
            READ(vertex->pos, v3);
            READ(vertex->normal, v3);
            READ(vertex->uv, v2);
            READ(vertex->color, v4);

            for (u32 i = 0; i < MAX_BONE_PER_VERTEX; ++i) { READ(vertex->node_ids[i], s32); }
            for (u32 i = 0; i < MAX_BONE_PER_VERTEX; ++i) { READ(vertex->node_weights[i], f32); }
        }

        READ(mesh->index_count, u32);
        READ_COUNT(mesh->indices, u32, mesh->index_count);

        READ(mesh->material_idx, u32);
    }

    //
    // Material
    //
    READ(model->material_count, u32);
    READ_COUNT(model->materials, Material, model->material_count);

    //
    // Nodes
    //
    READ(model->node_count, u32);
    if (model->node_count)
    {
        READ(model->root_bone_node_id, s32);
        model->nodes = push_array(arena, Node, model->node_count);

        for (u32 node_idx = 0;
             node_idx < model->node_count;
             ++node_idx)
        {
            Node *node = model->nodes + node_idx;

            READ(node->id, s32);
            READ(node->offset, m4x4);
            READ(node->base_transform, m4x4);
            READ(node->child_count, u32);
            READ_COUNT(node->child_ids, s32, node->child_count);
        }
    }

    Assert(at == end);
}

internal u32
animation_hash(u32 id, u32 length)
{
    // @TODO: Better hash function!
    u32 slot = ((id * 23 + id * 8) % length);
    return slot;
}
internal void
load_animation(Animation *anim, char *file_name, Memory_Arena *arena,
               Read_Entire_File *read_entire_file)
{
    Assert(anim);

    Entire_File entire_file = read_entire_file(file_name);
    Assert(entire_file.content_size);
    u8 *at  = (u8 *)entire_file.contents;
    u8 *end = at + entire_file.content_size;

    READ_COUNT(anim->name, char, string_length((char *)at) + 1);

    READ(anim->duration, f32);
    READ(anim->sample_count, u32);

    anim->samples = push_array(arena, Sample, anim->sample_count);
    for (u32 sample_idx = 0;
         sample_idx < anim->sample_count;
         ++sample_idx)
    {
        Sample *sample = anim->samples + sample_idx;

        READ(sample->id, s32);

        READ(sample->translation_count, u32);
        READ(sample->rotation_count, u32);
        READ(sample->scaling_count, u32);

        READ_COUNT(sample->translations, dt_v3_Pair, sample->translation_count);
        READ_COUNT(sample->rotations, dt_qt_Pair, sample->rotation_count);
        READ_COUNT(sample->scalings, dt_v3_Pair, sample->scaling_count);
    }

    Assert(at == end);

    //
    // Build hash-table (key: node_id, value: node_idx in Animation->nodes)
    //
    Animation_Hash_Table *ht = &anim->hash_table;
    ht->entry_count = anim->sample_count;
    ht->entries = push_array(arena, Animation_Hash_Entry, ht->entry_count);
    for (u32 sample_idx = 0;
         sample_idx < anim->sample_count;
         ++sample_idx)
    {
        Sample *sample = anim->samples + sample_idx;
        u32 entry_idx = animation_hash(sample->id, ht->entry_count);
        Animation_Hash_Entry *entry = ht->entries + entry_idx;
        Animation_Hash_Slot *slot = entry->first;
        if (slot)
        {
            while (slot->next)
            {
                slot = slot->next;
            }
            slot->next = push_struct(arena, Animation_Hash_Slot);
            Animation_Hash_Slot *new_slot = slot->next;
            new_slot->id = sample->id;
            new_slot->idx = sample_idx;
            new_slot->next = 0;
        }
        else
        {
            slot = push_struct(arena, Animation_Hash_Slot);
            slot->id = sample->id;
            slot->idx = sample_idx;
            slot->next = 0;

            entry->first = slot;
        }
    }
}
#undef READ
#undef READ_COUNT

//
// Font
//
internal void
load_font(Memory_Arena *arena, Read_Entire_File *read_file, Font *font)
{
    Entire_File read = read_file(ASSET_FILE_NAME);
    u8 *at = (u8 *)read.contents;
    u8 *end = at + read.content_size;

    // parse font header.
    Asset_Font_Header *header = (Asset_Font_Header *)at;
    at += sizeof(Asset_Font_Header);
    u32 kern_count = header->kerning_pair_count;
    font->v_advance = header->vertical_advance;
    font->ascent = header->ascent;
    font->descent = header->descent;
    font->max_width = header->max_width;

    // parse kerning pairs.
    for (u32 count = 0; count < kern_count; ++count) 
    {
        Asset_Kerning *asset_kern = (Asset_Kerning *)at;

        Kerning *kern = push_struct(arena, Kerning);
        *kern = {};
        kern->first = asset_kern->first;
        kern->second = asset_kern->second;
        kern->value = asset_kern->value;

        u32 entry_idx = kerning_hash(&font->kern_hashmap, kern->first, kern->second);
        push_kerning(&font->kern_hashmap, kern, entry_idx);
        at += sizeof(Asset_Kerning);
    }

    // parse glyphs.
    if (read.content_size != 0) 
    {
        while (at < end) 
        {
            Asset_Glyph *glyph = (Asset_Glyph *)at;
            Bitmap *bitmap = &glyph->bitmap;
            font->glyphs[glyph->codepoint] = glyph;
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
            Work_Memory_Arena *workSlot = begin_work_memory(trans_state);
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
