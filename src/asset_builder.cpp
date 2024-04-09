/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#define KB(N) (    N  * 1024ll )
#define MB(N) ( KB(N) * 1024ll )
#define GB(N) ( MB(N) * 1024ll )
#define TB(N) ( GB(N) * 1024ll )

#define Assert(exp) if (!(exp)) { *(volatile int *)0 = 0; }

#define __USE_WINDOWS       1
#define ALLOC_SIZE          GB(2)

#if __USE_WINDOWS
  #include <windows.h>
#else
  #define STB_TRUETYPE_IMPLEMENTATION
  #include "stb_truetype.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "intrinsics.h"
#include "asset.h"
#include "asset_builder.h"

#define PACK_MAGIC(a,b,c,d)     (a | (b << 8) | (c << 16) | (d << 24))          
#define MAGIC                   PACK_MAGIC('a','k','4','7')
#define FILE_VERSION            1

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
// Memory
//
inline void
init_memory() {
    size_t mem_size = ALLOC_SIZE;
    void *mem_block = malloc(mem_size);
    Assert(mem_block != 0);
    memset(mem_block, 0, ALLOC_SIZE);
    g_main_arena.base = mem_block;
    g_main_arena.size = mem_size;
}

inline void
deinit_memory() {
    free(g_main_arena.base);
}

inline void
flush(Memory_Arena *arena) {
    arena->used = 0;
}
///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
// Bitmap
//
static void
write_bmp(const char *filename, FILE *out_file) {
    FILE *file = fopen(filename, "rb");

    if(file) {
        BMP_Info_Header header = {};
        fread(&header, sizeof(BMP_Info_Header), 1, file);
        Assert(header.compression == 3);
        Assert(header.bpp == 32);

        fseek(file, header.bitmap_offset, SEEK_SET);

        Bitmap bitmap = {};
        bitmap.width = header.width;
        bitmap.height = header.height;
        u64 bitmap_size = bitmap.width * bitmap.height * 4;
        bitmap.memory = push_size(bitmap_size);
        fread(bitmap.memory, bitmap_size, 1, file);

        u32 r_mask = header.r_mask;
        u32 g_mask = header.g_mask;
        u32 b_mask = header.b_mask;
        u32 a_mask = ~(r_mask | g_mask | b_mask);        
        
        Bit_Scan_Result r_scan = find_least_significant_set_bit(r_mask);
        Bit_Scan_Result g_scan = find_least_significant_set_bit(g_mask);
        Bit_Scan_Result b_scan = find_least_significant_set_bit(b_mask);
        Bit_Scan_Result a_scan = find_least_significant_set_bit(a_mask);
        
        Assert(r_scan.found);
        Assert(g_scan.found);
        Assert(b_scan.found);
        Assert(a_scan.found);

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
    } else {
        printf("Couldn't open filename %s\n.", filename);
    }

    fclose(file);
}

static void *
read_entire_file(const char *filename) {
    void *result = 0;
    FILE *file = fopen(filename, "rb");
    if (file) {
        fseek(file, 0, SEEK_END);
        size_t filesize = ftell(file);
        fseek(file, 0, SEEK_SET);
        result = push_size(filesize);
        Assert(fread(result, filesize, 1, file) == 1);
        fclose(file);
    } else {
        printf("ERROR: Couldn't open file %s.\n", filename);
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
// Font Asset
//
static Asset_Glyph *
bake_glyph(HDC hdc, u32 codepoint, void *bits, s32 bi_width, s32 bi_height, TEXTMETRIC metric) {
    Asset_Glyph *result = push_struct(Asset_Glyph);
    wchar_t utf_codepoint = (wchar_t)codepoint;
    SIZE size;
    GetTextExtentPoint32W(hdc, &utf_codepoint, 1, &size);

    s32 width = size.cx;
    s32 height = size.cy;

    SetTextColor(hdc, RGB(0xff, 0xff, 0xff));
    TextOutW(hdc, 0, 0, &utf_codepoint, 1);

    s32 min_x = width - 1;
    s32 min_y = height - 1;
    s32 max_x = 0;
    s32 max_y = 0;

    u32 *pixel_row = (u32 *)bits + (bi_height - 1) * bi_width;
    for (s32 y = 0;
            y < height;
            ++y) {
        u32 *pixel = pixel_row;
        for (s32 x = 0;
                x < width;
                ++x) {
            // COLORREF pixel = GetPixel(hdc, x, y); // OS-call, but suck it.
            u8 gray = (u8)(*pixel++ & 0xff);
            if (gray != 0) {
                if (x < min_x) { min_x = x; }
                if (y < min_y) { min_y = y; }
                if (x > max_x) { max_x = x; }
                if (y > max_y) { max_y = y; }
            }
        }
        pixel_row -= bi_width;
    }

    s32 baseline = metric.tmAscent;
    s32 ascent;
#if 0
    if (min_y <= baseline) {
        ascent = baseline - min_y + 1;
    } else {
        ascent = 0;
    }
    if (max_y <= baseline) {
        descent = 0;
    } else {
        descent = max_y - baseline;
    }
    Assert(ascent >=0 && descent >= 0);
#else
    ascent = baseline - min_y + 1;
#endif

    s32 off_x = min_x;
    s32 off_y = min_y;

    s32 glyph_width = max_x - min_x + 1;
    s32 glyph_height = max_y - min_y + 1;
    s32 glyph_pitch = -glyph_width * 4;

    s32 margin = 1;

    result->codepoint = codepoint;
    result->ascent = ascent + margin;
    result->bitmap.width = glyph_width + margin * 2;
    result->bitmap.height = glyph_height + margin * 2;
    result->bitmap.pitch = -result->bitmap.width * 4;
    result->bitmap.handle = 0;
    result->bitmap.size = result->bitmap.height * result->bitmap.width * 4;
    result->bitmap.memory = push_size(result->bitmap.size);
    // memset(result->bitmap.memory, 0x7f, result->bitmap.size);

    u8 *dst_row = (u8 *)result->bitmap.memory + (result->bitmap.height - 1 - margin) * -result->bitmap.pitch + 4 * margin;
    pixel_row = (u32 *)bits + (bi_height - 1 - off_y) * bi_width;
    for (s32 y = 0;
            y < glyph_height;
            ++y) {
        u32 *dst_at = (u32 *)dst_row;
        u32 *pixel = pixel_row + off_x;
        for (s32 x = 0;
                x < glyph_width;
                ++x) {
            // COLORREF pixel = GetPixel(hdc, off_x + x, off_y + y);
            u8 gray = (u8)(*pixel++ & 0xff);
#if 0 // debug
            u32 c = (alpha << 24) |
                (gray << 16) |
                (gray <<  8) |
                gray;
#else
            u32 c = (gray << 24) |
                    (gray << 16) |
                    (gray <<  8) |
                     gray;
#endif
            *dst_at++ = c; 
        }
        dst_row += result->bitmap.pitch;
        pixel_row -= bi_width;
    }
    

#if 0
    u8 *read_buf = (u8 *)read_entire_file(filename);

    stbtt_fontinfo font;
    s32 init_ok = stbtt_InitFont(&font, read_buf, 0);
    if (init_ok) {
        int width, height;
        r32 scale = stbtt_ScaleForPixelHeight(&font, (r32)font_height);
        u8 *mono_bitmap = stbtt_GetCodepointBitmap(&font, 0, scale,
                codepoint, &width, &height, 0, 0);
        u8 *src_at = mono_bitmap;

        size_t size = width * height * 4;
        result->memory = push_size(size);
        result->width = width;
        result->height = height;
        result->pitch = -width * 4;
        result->size = width * height * 4;

        u8 *dst_row = (u8 *)result->memory + (height - 1) * -result->pitch;
        for (s32 y = 0;
                y < height;
                ++y) {
            u32 *dst_at = (u32 *)dst_row;
            for (s32 x = 0;
                    x < width;
                    ++x) {
                u8 gray = *src_at++;
                *dst_at++ = 0xff << 24 |
                            gray << 16 |
                            gray <<  8 |
                            gray;
            }
            dst_row += result->pitch;
        }
        stbtt_FreeBitmap(mono_bitmap, 0);
    }
#endif
    
    return result;
}

static void
error_handling(const char *str) {
    fputs(str, stderr);
    fputc('\n', stderr);
    exit(1);
}

static void
bake_font(const char *filename, const char *fontname, FILE* out, s32 cheese_height) {
    s32 bi_width = 1024;
    s32 bi_height = 1024;
    static HDC hdc = 0;
    TEXTMETRIC metric = {};
    ABC *ABCs = 0;
    void *bits = 0;
    u32 lo = ' ';
    u32 hi = '~';
    if (!hdc) {
        // add font file to resource pool.
        if (!AddFontResourceExA(filename, FR_PRIVATE, 0)) {
            error_handling("ERROR: Couldn't add font resource.");
        }

        // create font.
        HFONT font = CreateFontA(cheese_height, 0, 0, 0,
                FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                DEFAULT_PITCH|FF_DONTCARE, fontname);
        if (!font) { error_handling("ERROR: Couldn't create a font."); }

        hdc = CreateCompatibleDC(0);
        char temp[30];
        GetTextFaceA(hdc, 30, temp);
        BITMAPINFO info = {};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = bi_width;
        info.bmiHeader.biHeight = bi_height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        info.bmiHeader.biSizeImage = 0;
        info.bmiHeader.biXPelsPerMeter = 0;
        info.bmiHeader.biYPelsPerMeter = 0;
        info.bmiHeader.biClrUsed = 0;
        info.bmiHeader.biClrImportant = 0;
        HBITMAP bitmap = CreateDIBSection(hdc, &info, DIB_RGB_COLORS, &bits, 0, 0);
        SelectObject(hdc, bitmap);
        SelectObject(hdc, font);
        SetBkColor(hdc, RGB(0, 0, 0));

        GetTextMetrics(hdc, &metric);

        // kerning.
        s32 kern_count = GetKerningPairsA(hdc, 0, 0);
        KERNINGPAIR *kern_pairs = push_array(KERNINGPAIR, kern_count);
        GetKerningPairsA(hdc, kern_count, kern_pairs);

        // font header.
        Asset_Font_Header font_header = {};
        font_header.kerning_pair_count = kern_count;
        font_header.vertical_advance = metric.tmHeight + metric.tmInternalLeading + metric.tmExternalLeading;

        Asset_Kerning *asset_kern_pairs = push_array(Asset_Kerning, kern_count);
        for (s32 idx = 0;
                idx < kern_count;
                ++idx) {
            KERNINGPAIR *kern_pair = kern_pairs + idx;
            Asset_Kerning *asset_kern_pair = asset_kern_pairs + idx;
            asset_kern_pair->first = kern_pair->wFirst;
            asset_kern_pair->second = kern_pair->wSecond;
            asset_kern_pair->value = kern_pair->iKernAmount;
        }

        // ABC. What a convenient name.
        ABCs = push_array(ABC, hi - lo + 1);
        GetCharABCWidthsA(hdc, lo, hi, ABCs);

        // write font header.
        fwrite(&font_header, sizeof(font_header), 1, out);
        
        // write kerning pairs.
        fwrite(asset_kern_pairs, sizeof(*asset_kern_pairs) * kern_count, 1, out);



    }
    // write glyphs.
    for (u32 ch = lo;
            ch <= hi;
            ++ch) {
        Asset_Glyph *glyph = bake_glyph(hdc, ch, bits, bi_width, bi_height, metric);
        glyph->A = ABCs[ch - lo].abcA;
        glyph->B = ABCs[ch - lo].abcB;
        glyph->C = ABCs[ch - lo].abcC;
        fwrite(glyph, sizeof(*glyph), 1, out);
        fwrite(glyph->bitmap.memory, glyph->bitmap.size, 1, out);
    }
    
    flush(&g_main_arena); // important: danger if you will use multi-threading.
}

int
main(int argc, char **argv) {
    init_memory();

    Package package = {};
    package.header.magic = MAGIC;
    package.header.file_version = FILE_VERSION;
    package.header.flags = 0;
    package.header.table_of_contents_offset = sizeof(package.header);


    FILE *out = fopen("../data/asset.pack", "wb");
    if (out) {
        bake_font("C:/Windows/Fonts/cour.ttf", "Courier New", out, 30);


        fclose(out);
        printf("*** SUCCESSFUL! ***\n");
    } else {
        printf("ERROR: Couldn't open file.\n");
    }

    deinit_memory();
}
