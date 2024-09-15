R"MULTILINE(

in v4 persp_clip_P;
in v4 ortho_clip_P;

out v4 C;

layout(RGBA8) uniform image3D voxel_map;

void main()
{
    v3 idx_01 = 0.5f * (ortho_clip_P.xyz / ortho_clip_P.w + v3(1.0f));
    iv3 voxel_size = imageSize(voxel_map);
    iv3 idx = iv3(idx_01 * voxel_size);

    C = imageLoad(voxel_map, idx);
}

)MULTILINE"
