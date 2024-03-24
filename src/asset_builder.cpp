/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#define USE_WINDOWS 1

#ifdef USE_WINDOWS
#include <windows.h>
#else
#include "stb_truetype.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "types.h"
#include "intrinsics.h"
#include "asset_builder.h"

#define PACK_MAGIC(a,b,c,d)     (a | (b << 8) | (c << 16) | (d << 24))          
#define MAGIC                   PACK_MAGIC('a','k','4','7')
#define FILE_VERSION            1

static void
write_bitmap(const char *filename, FILE *out_file) {
    FILE *file = fopen(filename, "rb");

    if(file) {
        Bitmap_Info_Header header = {};
        fread(&header, sizeof(Bitmap_Info_Header), 1, file);
        assert(header.compression == 3);
        assert(header.bpp == 32);

        fseek(file, header.bitmap_offset, SEEK_SET);

        Bitmap bitmap = {};
        bitmap.width = header.width;
        bitmap.height = header.height;
        u64 bitmap_size = bitmap.width * bitmap.height * 4;
        bitmap.memory = malloc(bitmap_size);
        fread(bitmap.memory, bitmap_size, 1, file);

        u32 r_mask = header.r_mask;
        u32 g_mask = header.g_mask;
        u32 b_mask = header.b_mask;
        u32 a_mask = ~(r_mask | g_mask | b_mask);        
        
        Bit_Scan_Result r_scan = find_least_significant_set_bit(r_mask);
        Bit_Scan_Result g_scan = find_least_significant_set_bit(g_mask);
        Bit_Scan_Result b_scan = find_least_significant_set_bit(b_mask);
        Bit_Scan_Result a_scan = find_least_significant_set_bit(a_mask);
        
        assert(r_scan.found);
        assert(g_scan.found);
        assert(b_scan.found);
        assert(a_scan.found);

        s32 r_shift_down = (s32)r_scan.index;
        s32 g_shift_down = (s32)g_scan.index;
        s32 b_shift_down = (s32)b_scan.index;
        s32 a_shift_down = (s32)a_scan.index;
        
        u32 *src = (u32 *)bitmap.memory;
        for (s32 y = 0;
            y < header.height;
            ++y)
        {
            for (s32 x = 0;
                x < header.width;
                ++x)
            {
                u32 c = *src;

                r32 r = (r32)((c & r_mask) >> r_shift_down);
                r32 g = (r32)((c & g_mask) >> g_shift_down);
                r32 b = (r32)((c & b_mask) >> b_shift_down);
                r32 a = (r32)((c & a_mask) >> a_shift_down);

                // reformat bitmap file's RGBa to microsoft's stupid aRGB format
                // even though bmp is created by themselves.
                *src++ = (((u32)(a + 0.5f) << 24) |
                          ((u32)(r + 0.5f) << 16) |
                          ((u32)(g + 0.5f) <<  8) |
                          ((u32)(b + 0.5f) <<  0));
            }
        }

        fwrite(bitmap.memory, bitmap_size, 1, out_file);
        free(bitmap.memory);
    } else {
        printf("Couldn't open filename %s\n.", filename);
    }

    fclose(file);
}

static void
bake_glyph(const char *filename, const char *fontname, u32 codepoint) {
#ifdef USE_WINDOWS
    static HDC hdc = 0;
    if (!hdc) {
        AddFontResourceExA(filename, FR_PRIVATE, 0);
        HFONT font = CreateFontA(128, 0, 0, 0,
                FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                DEFAULT_PITCH|FF_DONTCARE, fontname);
        hdc = CreateCompatibleDC(hdc);
        HBITMAP bitmap = CreateCompatibleBitmap(hdc, 1024, 1024);
        SelectObject(hdc, bitmap);
        SetBkColor(hdc, RGB(0, 0, 0));

        TEXTMETRIC text_metric;
        GetTextMetrics(hdc, &text_metric);
    }

    wchar_t utf_codepoint = (wchar_t)codepoint;

    SIZE size;
    GetTextExtentPoint32W(hdc, &utf_codepoint, 1, &size);

    s32 width = size.cx;
    s32 height = size.cy;

    PatBlt(hdc, 0, 0, width, height, BLACKNESS);
    SetTextColor(hdc, RGB(0xff, 0xff, 0xff));

    TextOutW(hdc, 0, 0, &utf_codepoint, 1);
#else // stb_truetype.h
#endif
}

int
main(int argc, char **argv) {
    Package package = {};
    package.header.magic = MAGIC;
    package.header.file_version = FILE_VERSION;
    package.header.flags = 0;
    package.header.table_of_contents_offset = sizeof(package.header);


    FILE *out = fopen("../data/asset.pack", "wb");
    if (out) {
        fwrite(&package, sizeof(package), 1, out);

        write_bitmap("../data/white_particle.bmp", out);






        fclose(out);
        printf("*** SUCCESSFUL! ***\n");
    } else {
        printf("ERROR: Couldn't open file.\n");
    }

}
