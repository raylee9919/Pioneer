/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright %s by Sung Woo Lee. All Rights Reserved. $
   ======================================================================== */
struct Memory_Arena 
{
    size_t  size;
    void    *base;
    size_t  used;
};

Memory_Arena g_main_arena;

#define array_count(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

#define push_array(STRUCT, COUNT) (STRUCT *)push_size(sizeof(STRUCT) * COUNT)
#define push_struct(STRUCT) (STRUCT *)push_size(sizeof(STRUCT))
static void *
push_size(size_t size) 
{
    void *result = 0;
    if (size + g_main_arena.used > g_main_arena.size) {
        printf("ERROR: Not enough memory!\n");
        exit(1);
    }
    result = (u8 *)g_main_arena.base + g_main_arena.used;
    g_main_arena.used += size;

    return result;
}
