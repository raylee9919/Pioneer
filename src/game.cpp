/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */


#define ASSET_FILE_NAME "asset.pack"

#define MESH_DIR "mesh/"

#include "types.h"
#include "game.h"
#include "memory.cpp"
#include "render_group.cpp"
#include "sim.cpp"
#include "asset.cpp"
#include "animation_player.cpp"

#define GRASS_COUNT_MAX 100'000
#define GRASS_DENSITY 10
#define GRASS_RANDOM_OFFSET 0.10f
#define TURBULENCE_MAP_SIDE 256 

#define STAR_COUNT_MAX 100'000



#if __DEVELOPER
global_var Game_Memory *g_debug_memory;
#endif

internal void
init_console(Console *console, f32 screen_height, f32 screen_width, Font *font) //@TODO: assert font...
{
    Assert(!console->initted);

    console->half_dim   = v2{600.f, 250.0f};
    console->dt         = 0.0f;
    console->is_down    = false;
    console->bg_color   = v4{0.02f, 0.02f, 0.02f, 0.9f};
    console->font       = font;

    console->input_baseline_offset = v2{5.0f, 2.0f + font->descent};

    Console_Cursor *cursor = &console->cursor;
    cursor->offset = console->input_baseline_offset;
    cursor->dim = v2{font->max_width, font->ascent + font->descent + 1.0f};
    cursor->color1 = v4{0.2f, 0.2f, 0.2f, 1.0f};
    cursor->color2 = v4{1.0f, 1.0f, 1.0f, 1.0f};
}

internal void
accumulate(Console *console, f32 dt)
{
    if (console->cooltime > 0.0f)
        console->cooltime -= dt;

    console->dt += (console->is_down ? 1.0f : -1.0f) * dt;
    console->dt = clamp(console->dt, 0.0f, CONSOLE_TARGET_T);
}

internal void
interpolate(Model *model, Animation *anim1, f32 dt1, f32 t, Animation *anim2, f32 dt2)
{
    for (s32 id = 0;
         id < (s32)model->node_count;
         ++id)
    {
        Node *node = model->nodes + id;
        Node_Hash_Result res1 = get_sample_index(anim1, id);
        Node_Hash_Result res2 = get_sample_index(anim2, id);

        if (res1.found && res2.found)
        {
            Sample *sample1 = anim1->samples + res1.idx;
            Sample *sample2 = anim2->samples + res2.idx;
            Assert(sample1->id == id && sample1->id == sample2->id);

            TRS trs1 = interpolate_sample(sample1, dt1);
            TRS trs2 = interpolate_sample(sample2, dt2);
            TRS r = interpolate_trs(trs1, t, trs2);
            m4x4 transform = trs_to_transform(r.translation, r.rotation, r.scaling);
            node->current_transform = transform;
        }
        else
        {
            node->current_transform = node->base_transform;
        }
    }
}

extern "C"
GAME_UPDATE(game_update)
{

#if __DEVELOPER
    g_debug_memory = game_memory;
#endif

    if (!game_state->init) 
    {
        init_arena(&game_state->world_arena,
                   game_memory->permanent_memory_size - sizeof(Game_State),
                   (u8 *)game_memory->permanent_memory + sizeof(Game_State));

        game_state->world               = push_struct(&game_state->world_arena, World);
        World *world                    = game_state->world;
        world->chunk_dim                = v3{10.0f, 3.0f, 10.0f};
        Memory_Arena *world_arena       = &game_state->world_arena;
        Chunk_Hashmap *chunk_hashmap    = &game_state->world->chunkHashmap;

        game_state->random_series = seed(1219);

        Chunk *chunk = get_chunk(&game_state->world_arena, chunk_hashmap, Chunk_Position{0, 0, 0});
        v3 dim = _v3_(1.0f, 1.0f, 1.0f);

        game_state->grass_world_transforms = push_array(&game_state->world_arena, m4x4, GRASS_COUNT_MAX); 

        s32 hX = round_f32_to_s32(game_state->world->chunk_dim.x * 0.5f);
        s32 hZ = round_f32_to_s32(game_state->world->chunk_dim.z * 0.5f);
        for (s32 X = -hX; X <= hX; ++X) 
        {
            for (s32 Z = -hZ; Z <= hZ; ++Z) 
            {
                Chunk_Position tile_pos = {chunk->x, chunk->y, chunk->z};
                tile_pos.offset.x += dim.x * X;
                tile_pos.offset.z += dim.z * Z;
                recalc_pos(&tile_pos, game_state->world->chunk_dim);
                Entity *tile1 = push_entity(world_arena, chunk_hashmap,
                                            Entity_Type::TILE, tile_pos, world->chunk_dim);
#if 1
                for (s32 z = -GRASS_DENSITY; z <= GRASS_DENSITY; ++z)
                {
                    for (s32 x = -GRASS_DENSITY; x <= GRASS_DENSITY; ++x)
                    {
                        Chunk_Position grass_pos = tile_pos;
                        grass_pos.offset.z += ((1.0f / GRASS_DENSITY) * dim.z * z +
                                               rand_range(&game_state->random_series, -GRASS_RANDOM_OFFSET, GRASS_RANDOM_OFFSET));
                        grass_pos.offset.x += ((1.0f / GRASS_DENSITY) * dim.x * x + 
                                               rand_range(&game_state->random_series, -GRASS_RANDOM_OFFSET, GRASS_RANDOM_OFFSET));
                        recalc_pos(&grass_pos, game_state->world->chunk_dim);

                        v3 translation      = _v3_(grass_pos.x * world->chunk_dim.x + grass_pos.offset.x,
                                                   grass_pos.y * world->chunk_dim.y + grass_pos.offset.y,
                                                   grass_pos.z * world->chunk_dim.z + grass_pos.offset.z);
                        f32 theta           = rand_range(&game_state->random_series, 0, pi32 * 0.5f);
                        qt rotation         = _qt_(cos(theta), 0, sin(theta), 0);
                        f32 scale           = rand_range(&game_state->random_series, 0.75f, 1.0f);
                        v3 scaling          = _v3_(scale, scale, scale);

                        Assert(game_state->grass_count != GRASS_COUNT_MAX);
                        game_state->grass_world_transforms[game_state->grass_count++] = transpose(trs_to_transform(translation, rotation, scaling));
                    }
                }
#endif
            }
        }


        Entity *xbot = push_entity(world_arena, chunk_hashmap, Entity_Type::XBOT, Chunk_Position{0, 0, 0}, world->chunk_dim);
        game_state->player = xbot;


        f32 T = pi32 * 0.1f;
        f32 D = 5;
        // @TODO: is it too janky?
        game_state->free_camera = push_camera( &game_state->world_arena,
                                                eCamera_Type_Perspective,
                                                (f32)game_screen_buffer->width, (f32)game_screen_buffer->height,
                                                0.5f,
                                                _v3_(0, D * sin(T * 2.0f), D * cos(T * 2.0f)),
                                                _qt_(cos(T), -sin(T), 0, 0) );

        game_state->main_camera = push_camera( &game_state->world_arena,
                                                eCamera_Type_Perspective,
                                                (f32)game_screen_buffer->width, (f32)game_screen_buffer->height,
                                                0.5f,
                                                _v3_(0, D * sin(T * 2.0f), D * cos(T * 2.0f)),
                                                _qt_(cos(T), -sin(T), 0, 0) );

        game_state->orthographic_camera = push_camera( &game_state->world_arena,
                                                       eCamera_Type_Orthographic,
                                                       (f32)game_screen_buffer->width, (f32)game_screen_buffer->height);




        // Star
        game_state->star_world_transforms = push_array(&game_state->world_arena, m4x4, STAR_COUNT_MAX);
#define STAR_COUNT 10'000
#define STAR_SCALE 0.2f
#define STAR_DIST  200.0f
        for (u32 cnt = 0;
             cnt < STAR_COUNT;
             ++cnt)
        {
            Assert(game_state->star_count < STAR_COUNT_MAX);
            f32 x = rand_bilateral(&game_state->random_series);
            f32 y = rand_bilateral(&game_state->random_series);
            f32 z = rand_bilateral(&game_state->random_series);
            f32 theta = rand_range(&game_state->random_series, 0.0f, pi32 * 0.5f);
            v3 v = normalize(_v3_(x, y, z));
            v3 translation  = STAR_DIST * v;
            qt rotation     = _qt_(cos(theta), sin(theta) * v);
            v3 scaling      = _v3_(STAR_SCALE, STAR_SCALE, STAR_SCALE);
            game_state->star_world_transforms[game_state->star_count++] = transpose(trs_to_transform(translation, rotation, scaling));
        }


        // @TEMPORARY
        game_state->light = push_entity(world_arena, chunk_hashmap, Entity_Type::LIGHT, {}, world->chunk_dim);

        game_state->init = true;
    }

    f32 dt = game_input->dt;
    game_state->time += dt;
    f32 time = game_state->time;

    f32 height = (f32)game_screen_buffer->height;
    f32 width = (f32)game_screen_buffer->width;

    Entity *player = game_state->player;

    DEBUG_VARIABLE(f32, Xbot, Accel_Constant);
    player->u = Accel_Constant;

    // @TEMPORARY
    game_state->light->chunk_pos = Chunk_Position{0, 0, 0, v3{0, 2.0f, 0}};

    ///////////////////////////////////////////////////////////////////////////
    //
    // Init Transient Memory
    //
    void *transient_memory  = game_memory->transient_memory;
    u64 transient_memory_capacity = game_memory->transient_memory_size;
    Assert(sizeof(Transient_State) < transient_memory_capacity);
    Transient_State *transient_state = (Transient_State *)transient_memory;

    if (!transient_state->init)
    {
        init_arena(&transient_state->transient_arena, MB(200),
                   (u8 *)transient_memory + sizeof(Transient_State));

        // reserved memory for multi-thread work data.
        for (u32 idx = 0;
             idx < array_count(transient_state->work_arenas);
             ++idx) 
        {
            Work_Memory_Arena *work_slot = transient_state->work_arenas + idx;
            init_sub_arena(&work_slot->arena, &transient_state->transient_arena, MB(4));
        }

        // asset arena.
        init_arena(&transient_state->asset_arena, MB(200),
                   (u8 *)transient_memory + sizeof(Transient_State) + transient_state->transient_arena.size);

        transient_state->high_priority_queue    = game_memory->high_priority_queue;
        transient_state->low_priority_queue     = game_memory->low_priority_queue;
        Game_Assets *game_assets            = &transient_state->game_assets; // TODO: Ain't thrilled about it.
        game_assets->read_entire_file       = game_memory->platform.debug_platform_read_file;

#if __DEVELOPER
        game_assets->xbot_model = push_struct(&transient_state->asset_arena, Model);
        game_assets->cube_model = push_struct(&transient_state->asset_arena, Model);
        game_assets->octahedral_model = push_struct(&transient_state->asset_arena, Model);
        game_assets->sphere_model = push_struct(&transient_state->asset_arena, Model);
        game_assets->grass_model = push_struct(&transient_state->asset_arena, Model);

        game_assets->xbot_idle = push_struct(&transient_state->asset_arena, Animation);
        load_animation(game_assets->xbot_idle, "animation/xbot_idle.sanm", &transient_state->asset_arena, game_memory->platform.debug_platform_read_file);

        game_assets->xbot_run = push_struct(&transient_state->asset_arena, Animation);
        load_animation(game_assets->xbot_run, "animation/xbot_run.sanm", &transient_state->asset_arena, game_memory->platform.debug_platform_read_file);
#endif
        // @Temporary
        // @Temporary
        // @Temporary
        load_model(game_assets->xbot_model, MESH_DIR"xbot.smsh", &transient_state->asset_arena, game_memory->platform.debug_platform_read_file);
        player->animation_transform = push_array(&transient_state->transient_arena, m4x4, game_assets->xbot_model->node_count);
        f32 xbot_scale = 0.01f;
        game_assets->xbot_model->nodes[0].base_transform =
            scale(game_assets->xbot_model->nodes[0].base_transform, xbot_scale * v3{1, 1, 1});
        player->animation_channels[0].animation = game_assets->xbot_idle;

        load_model(game_assets->cube_model, "cube.smsh", &transient_state->asset_arena, game_memory->platform.debug_platform_read_file);
        load_model(game_assets->octahedral_model, "octahedral.smsh", &transient_state->asset_arena, game_memory->platform.debug_platform_read_file);
        load_model(game_assets->sphere_model, MESH_DIR"sphere.smsh", &transient_state->asset_arena, game_memory->platform.debug_platform_read_file);
        load_model(game_assets->grass_model, "grass.smsh", &transient_state->asset_arena, game_memory->platform.debug_platform_read_file);
        for (u32 vertex_idx = 0;
             vertex_idx < game_assets->grass_model->meshes[0].vertex_count;
             ++vertex_idx)
        {
            Vertex *vertex = game_assets->grass_model->meshes[0].vertices + vertex_idx;
            game_assets->grass_max_vertex_y = maximum(game_assets->grass_max_vertex_y, vertex->pos.y);
        }



        game_assets->star_mesh = game_assets->octahedral_model->meshes;

        load_font(&transient_state->asset_arena, game_memory->platform.debug_platform_read_file, &game_assets->debug_font);

        //
        // Noise Map
        //
        game_assets->turbulence_map = load_bmp(&transient_state->asset_arena, game_memory->platform.debug_platform_read_file, "turbulence.bmp");

#if __DEVELOPER
        game_assets->debug_bitmap = load_bmp(&transient_state->asset_arena, game_memory->platform.debug_platform_read_file, "doggo.bmp");
#endif

        if (!game_state->console.initted)
            init_console(&game_state->console, width, height, &game_assets->debug_font);

        transient_state->init = true;  
    }

    Game_Assets *game_assets = &transient_state->game_assets;

    Temporary_Memory render_memory = begin_temporary_memory(&transient_state->transient_arena);




    v3 chunk_dim = game_state->world->chunk_dim;

    //
    // Process Input
    //
    Render_Group *render_group = 0;
    DEBUG_IF(Render_Use_Debug_Camera)
    {
        render_group = alloc_render_group(&transient_state->transient_arena, MB(16),
                                                        game_state->free_camera);
        Camera *free_camera = game_state->free_camera;
        f32 C = dt * 3.0f;
        if (game_input->W.is_down)
        {
            m4x4 rotation = to_m4x4(free_camera->world_rotation);
            free_camera->world_translation += rotation * _v3_(0, 0, -C);
        }
        if (game_input->S.is_down)
        {
            m4x4 rotation = to_m4x4(free_camera->world_rotation);
            free_camera->world_translation += rotation * _v3_(0, 0, C);
        }
        if (game_input->D.is_down)
        {
            m4x4 rotation = to_m4x4(free_camera->world_rotation);
            free_camera->world_translation += rotation * _v3_(C, 0, 0);
        }
        if (game_input->A.is_down)
        {
            m4x4 rotation = to_m4x4(free_camera->world_rotation);
            free_camera->world_translation += rotation * _v3_(-C, 0, 0);
        }

        if (game_input->Q.is_down)
        {
            m4x4 rotation = to_m4x4(free_camera->world_rotation);
            free_camera->world_translation += rotation * _v3_(0, -C, 0);
        }
        if (game_input->E.is_down)
        {
            m4x4 rotation = to_m4x4(free_camera->world_rotation);
            free_camera->world_translation += rotation * _v3_(0, C, 0);
        }
    }
    else
    {
        Camera *main_camera = game_state->main_camera;
        render_group = alloc_render_group(&transient_state->transient_arena, MB(16),
                                                        game_state->main_camera);

        if (game_input->W.is_down)
        {
            m4x4 rotation = to_m4x4(player->world_rotation);
            player->accel = rotation * _v3_(0, 0, dt * player->u);
        }
        if (game_input->D.is_down)
        {
            player->world_rotation = _qt_(cos(dt), 0, -sin(dt), 0) * player->world_rotation;
        }
        if (game_input->A.is_down)
        {
            player->world_rotation = _qt_(cos(dt), 0, sin(dt), 0) * player->world_rotation;
        }
    }
    Assert(render_group);

    Render_Group *orthographic_group = alloc_render_group(&transient_state->transient_arena, MB(16),
                                                          game_state->orthographic_camera);

    // @Developer
    // Drop-down console.
    Console *console = &game_state->console;
    accumulate(console, dt);
    if (game_input->tilde.is_down &&
        console->cooltime <= 0.0f)
    {
        console->is_down = !console->is_down;
        console->cooltime = CONSOLE_COOLTIME;
    }
    if (console->cbuf_at < array_count(console->cbuf) - 1)
    {
        if (game_input->A.is_down)
        {
            console->cbuf[console->cbuf_at++] = 'a';
            console->cbuf[console->cbuf_at] = 0;
        }
    }
    if (console->dt != 0.0f)
    {
        f32 T = normalize(0.0f, console->dt, CONSOLE_TARGET_T);
        f32 next_y;

        next_y = lerp(width, T, height - console->half_dim.y);
        Rect2 console_rect = rect2_cen_half_dim(v2{width * 0.5f, next_y}, console->half_dim);
        v2 con_o = console_rect.min;
        push_rect(orthographic_group, console_rect, 0.0f, console->bg_color);

        Console_Cursor *cursor = &console->cursor;
        Rect2 cursor_rect = rect2_min_dim(con_o + cursor->offset - v2{0.0f, console->font->descent}, cursor->dim);
        v4 cursor_color = lerp(cursor->color1, 0.5f*sin(time*5.0f) + 1.0f, cursor->color2);
        push_rect(orthographic_group, cursor_rect, 0.0f, cursor_color);

        Rect2 r = string_op(String_Op::DRAW | String_Op::GET_RECT,
                            orthographic_group, _v3_(con_o + console->input_baseline_offset, 0.0f),
                            console->cbuf, console->font);
        cursor->offset = console->input_baseline_offset + v2{r.max.x - r.min.x};
    }
    else
    {
        // No need to draw.
    }


    //
    //
    //


    Chunk_Position camPos = {};
    v3 camDim = v3{100.0f, 5.0f, 50.0f};
    Chunk_Position min_pos = camPos;
    Chunk_Position max_pos = camPos;
    min_pos.offset -= 0.5f * camDim;
    max_pos.offset += 0.5f * camDim;
    recalc_pos(&min_pos, chunk_dim);
    recalc_pos(&max_pos, chunk_dim);


    //
    // Update entities
    //
    update_entities(game_state, dt, min_pos, max_pos);


    //
    // Draw
    //
#if 1
    for (s32 Z = min_pos.z;
         Z <= max_pos.z;
         ++Z) 
    {
        for (s32 Y = min_pos.y;
             Y <= max_pos.y;
             ++Y) 
        {
            for (s32 X = min_pos.x;
                 X <= max_pos.x;
                 ++X) 
            {
                Chunk *chunk = get_chunk(&game_state->world_arena,
                                         &game_state->world->chunkHashmap,
                                         Chunk_Position{X, Y, Z});
                for (Entity *entity = chunk->entities.head;
                     entity != 0;
                     entity = entity->next) 
                {

                    m4x4 world_transform = trs_to_transform(entity->world_translation,
                                                            entity->world_rotation,
                                                            entity->world_scaling);

                    switch (entity->type) 
                    {
                        case Entity_Type::XBOT: 
                        {
                            Model *model = game_assets->xbot_model;
                            if (model)
                            {
                                f32 scalar = len(entity->velocity);
                                f32 lo = epsilon_f32;
                                f32 hi = 0.7f;
                                Animation_Channel *channel = &entity->animation_channels[0];

                                if (scalar <= lo)
                                {
                                    Animation *new_anim = game_assets->xbot_idle;
                                    if (channel->animation != new_anim)
                                    {
                                        channel->animation = new_anim;
                                        channel->dt = 0.0f;
                                    }
                                    eval(model, channel->animation, channel->dt, entity->animation_transform, true);
                                    accumulate(channel, dt);
                                }
                                else if (scalar > hi)
                                {
                                    Animation *new_anim = game_assets->xbot_run;
                                    if (channel->animation != new_anim)
                                    {
                                        channel->animation = new_anim;
                                        channel->dt = 0.0f;
                                    }
                                    eval(model, channel->animation, channel->dt, entity->animation_transform, true);
                                    accumulate(channel, dt);
                                }
                                else
                                {
                                    f32 t = (scalar - lo) / (hi - lo);
                                    if (channel->animation == game_assets->xbot_idle)
                                    {
                                        interpolate(model,
                                                    channel->animation, channel->dt,
                                                    t,
                                                    game_assets->xbot_run, 0.0f);
                                    }
                                    else
                                    {
                                        interpolate(model,
                                                    game_assets->xbot_idle, 0.0f,
                                                    t,
                                                    channel->animation, channel->dt);
                                    }
                                    eval(model, 0, 0, entity->animation_transform, false);
                                }

                                for (u32 mesh_idx = 0;
                                     mesh_idx < model->mesh_count;
                                     ++mesh_idx)
                                {
                                    Mesh *mesh = model->meshes + mesh_idx;
                                    Material *mat = model->materials + mesh->material_idx;
                                    v3 light_pos = subtract(game_state->light->chunk_pos, {}, game_state->world->chunk_dim);
                                    push_mesh(render_group, mesh, mat, world_transform, light_pos, entity->animation_transform);
                                }
                            }
                        } break;

                        case Entity_Type::TILE: 
                        {
                            Model *model = game_assets->cube_model;
                            if (model)
                            {
                                for (u32 mesh_idx = 0;
                                     mesh_idx < model->mesh_count;
                                     ++mesh_idx)
                                {
                                    Mesh *mesh = model->meshes + mesh_idx;
                                    Material *mat = model->materials + mesh->material_idx;
                                    v3 light_pos = subtract(game_state->light->chunk_pos, {}, game_state->world->chunk_dim);
                                    push_mesh(render_group, mesh, mat, world_transform, light_pos);
                                }
                            }
                        } break;

                        case Entity_Type::LIGHT:
                        {
                            Model *model = game_assets->sphere_model;
                            if (model)
                            {
                                for (u32 mesh_idx = 0;
                                     mesh_idx < model->mesh_count;
                                     ++mesh_idx)
                                {
                                    Mesh *mesh = model->meshes + mesh_idx;
                                    Material *mat = model->materials + mesh->material_idx;
                                    v3 light_pos = subtract(game_state->light->chunk_pos, {}, game_state->world->chunk_dim);
                                    push_mesh(render_group, mesh, mat, world_transform, light_pos);
                                }
                            }
                        } break;

                        INVALID_DEFAULT_CASE
                    }

                }
            }
        }
    }
#endif


    DEBUG_IF(Render_DrawGrass)
    {
        push_grass(render_group, &game_assets->grass_model->meshes[0], game_state->grass_count, game_state->grass_world_transforms, time, game_assets->grass_max_vertex_y, game_assets->turbulence_map);
    }

    DEBUG_IF(Render_DrawStar)
    {
        push_star(render_group, game_assets->star_mesh, game_state->star_count, game_state->star_world_transforms, time);
    }


#if __DEVELOPER && 0
    if (DEBUG_UI_ENABLED)
    {
        Debug_ID entity_debug_id = DEBUG_POINTER_ID(player);

        DEBUG_BEGIN_DATA_BLOCK("player entity", entity_debug_id);
        DEBUG_VALUE(player->world_translation);
        DEBUG_VALUE(player->velocity);
        DEBUG_VALUE(player->accel);
        DEBUG_VALUE(player->u);
        DEBUG_VALUE(game_assets->debug_bitmap);
        DEBUG_END_DATA_BLOCK();
    }
#endif

    
    render_group_to_output_batch(render_group, &game_memory->render_batch);
    render_group_to_output_batch(orthographic_group, &game_memory->render_batch);
    end_temporary_memory(&render_memory);
}


#if __DEVELOPER
#include "debug.cpp"
#else

extern "C"
DEBUG_FRAME_END(debug_frame_end) 
{
    return 0;
}
#endif
