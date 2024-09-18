R"MULTILINE(

in v3 clip_P;

out v4 C;

void main()
{
    C = v4(0.5f * clip_P + 0.5f, 1.0f);
}
)MULTILINE"
