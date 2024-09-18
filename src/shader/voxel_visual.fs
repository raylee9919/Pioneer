R"MULTILINE(

in v4 vP;
in v4 ortho_clip_P;

out v4 C;

layout(binding = 0, rgba8) uniform volatile coherent image3D voxel_map;
layout(binding = 1, r32ui) uniform volatile coherent uimage3D albedo_map;
layout(binding = 2, r32ui) uniform volatile coherent uimage3D normal_map;

void main()
{
    v3 idx_01 = 0.5f * ortho_clip_P.xyz + v3(0.5f);
    iv3 voxel_size = imageSize(voxel_map);
    iv3 idx = iv3(idx_01 * voxel_size);

#if 1
    uvec4 A = imageLoad(albedo_map, idx);
    v4 result = rgba8_to_v4(A.x);
    C = result;
#else
    uvec4 N = imageLoad(normal_map, idx);
    v4 result = rgba8_to_v4(N.x);
    C = result;
#endif
}

)MULTILINE"
