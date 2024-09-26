R"MULTILINE(

in v4 vP;
in v4 ortho_clip_P;

out v4 C;

uniform layout(binding = 0, r32ui) uimageBuffer octree_nodes;
uniform u32 octree_level;

void main()
{
    v3 idx_01 = 0.5f * ortho_clip_P.xyz + v3(0.5f);
    u32 res = ((1 << octree_level) - 1);
    uv3 coord = uv3(idx_01 * res);
    u32 idx = 0;
    u32 node = 0;

   for (u32 level = 0;
        level < octree_level;
        ++level)
   {
       u32 s = (octree_level - level);
       uv3 sub = coord;
       sub >>= s;
       u32 offset = sub.x + (sub.y << 1) + (sub.z << 2);
       sub <<= s;
       coord -= sub;
       idx = node + offset;

       node = (imageLoad(octree_nodes, s32(idx)).r & 0x7fffffff);
   }

    u32 val = (imageLoad(octree_nodes, s32(node)).r);
    if ((val & 0x80000000) != 0)
    {
        C = v4(1);
    }
    else
    {
        C = v4(1, 0, 0, 1);
    }
}

)MULTILINE"
