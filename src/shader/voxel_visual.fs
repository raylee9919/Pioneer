R"MULTILINE(

in v4 clip_P;

out v4 C;

layout(RGBA8) uniform image3D voxel_map;

void main()
{
    v3 idx_01 = 0.5f * (clip_P.xyz / clip_P.w + v3(1.0f));
    iv3 voxel_size = imageSize(voxel_map);
    iv3 idx = iv3(idx_01 * voxel_size);

#if 1
    C = imageLoad(voxel_map, idx);
#else
    C = v4(idx_01, 1.0f);
#endif
}

)MULTILINE"
