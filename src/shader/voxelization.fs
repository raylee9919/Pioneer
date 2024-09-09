R"MULTILINE(

out v4 result;

in v3 fP;
in v3 fN;

struct Material
{
    v3 color;
    f32 ambient;
    f32 diffuse;
    f32 specular;
};

struct Light
{
    v3 P;
    v3 color;
};

uniform u32 light_count;
uniform Material material;
layout(RGBA8) uniform image3D voxel_map;

void main()
{
    v3 result = v3(0, 0, 0);

    for ()
    {
        v3 ambient = material.ambient * light.color;

        v3 N = normalize(fN);
        v3 light_dir = normalize(light.P - fP);
        f32 cos_falloff = max(dot(N, light_dir), 0.0f);
        v3 diffuse = material.diffuse * cos_falloff * light.color;

        result += (ambient + diffuse) * material.color;
    }

    // Store value into voxel-map.
    ivec3 voxel_size = imageSize(tex3D);
    v3 fP_normalized = 0.5f * (fP + v3(1.0f));
    ivec3 location = ivec3(fP_normalized * voxel_size);
    image_store(voxel_map, location, vec4(result, 1.0f));
}

)MULTILINE"
