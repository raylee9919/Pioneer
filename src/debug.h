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
    // todo: fix multi-threading in cycles.
    const char *filename;
    const char *function;
    s32 line;

    u64 cycles;
    s32 hit_count;

    char *debug_str;
};

Debug_Counter g_debug_counters[];

struct Timed_Block {
    Debug_Counter *debug_counter;
    u64 start_cycles;

    Timed_Block(s32 counter, const char *filename, const char *function, s32 line) {
        this->debug_counter = g_debug_counters + counter;

        debug_counter->filename = filename;
        debug_counter->function = function;
        debug_counter->line = line;

        start_cycles = __rdtsc();
        debug_counter->hit_count++;
    }

    ~Timed_Block() {
        this->debug_counter->cycles += (__rdtsc() - start_cycles);
    }
};


#define DEBUG_H
#endif
