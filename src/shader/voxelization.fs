R"MULTILINE(

layout(RGBA8) uniform image3D voxel_map;

// @TEMPORARY
v3 light_P = v3(0.0f, 2.0f, 0.0f);

in v4 clip_P;

void main()
{
    v3 idx_01 = 0.5f * (clip_P.xyz / clip_P.w + v3(1.0f));
    iv3 voxel_size = imageSize(voxel_map);
    iv3 idx = iv3(idx_01 * voxel_size);
#if 0
    imageStore(voxel_map, idx, v4(idx_01, 1));
#else
    imageStore(voxel_map, idx, v4(1, 0, 1, 1));
#endif
}

)MULTILINE"
