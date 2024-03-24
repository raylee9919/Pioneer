#ifndef DEBUG_H
 /* ―――――――――――――――――――――――――――――――――――◆――――――――――――――――――――――――――――――――――――
    $File: $
    $Date: $
    $Revision: $
    $Creator: Sung Woo Lee $
    $Notice: (C) Copyright 2024 by Sung Woo Lee. All Rights Reserved. $
    ―――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――――― */

#define TIMED_BLOCK(ID) Timed_Block timed_block_##ID(DebugCycleCounter_##ID);
struct Timed_Block {
    u32 id;

    Timed_Block(u32 id) {
        this->id = id;
        rdtsc_begin(g_debug_cycle_counters, id);
    }

    ~Timed_Block() {
        rdtsc_end(g_debug_cycle_counters, this->id);
    }
};


#define DEBUG_H
#endif
