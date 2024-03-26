#ifndef DEBUG_H
/* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
   $File: $
   $Date: $
   $Revision: $
   $Creator: Sung Woo Lee $
   $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
   ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#define TIMED_BLOCK() \
    Timed_Block timed_block_##__LINE__(__COUNTER__, __FILE__, __FUNCTION__, __LINE__)

struct Debug_Counter {
    u64 cycles;
    const char *filename;
    const char *function;
    s32 line;
    s32 hit_count;

    char *debug_str;
};

Debug_Counter g_debug_counters[];

struct Timed_Block {
    s32 idx;
    Debug_Counter *debug_counter;

    Timed_Block(s32 counter, const char *filename, const char *function, s32 line) {
        this->idx = counter;
        this->debug_counter = g_debug_counters + counter;
        this->debug_counter->cycles = 0;
        this->debug_counter->cycles -= __rdtsc();
        this->debug_counter->filename = filename;
        this->debug_counter->function = function;
        this->debug_counter->line = line;
        this->debug_counter->hit_count += 1;
    }

    ~Timed_Block() {
        this->debug_counter->cycles += __rdtsc();
    }
};


#define DEBUG_H
#endif
