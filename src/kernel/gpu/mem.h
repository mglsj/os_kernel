#pragma once

#define GPU_CACHED_BASE 0x40000000
#define GPU_UNCACHED_BASE 0xC0000000
#define GPU_MEM_BASE GPU_UNCACHED_BASE

#define BUS_ADDRESS(addr) (((addr) & ~0xC0000000) | GPU_MEM_BASE)

#define MB (1024 * 1024)
extern char reserved_mem_start; // 60 MB

#define TERMINAL_BUFFER_LOCATION (&reserved_mem_start) // 20 MB
#define TERMINAL_BUFFER_SIZE (20 * MB)

#define DMA_MEM_LOCATION (TERMINAL_BUFFER_LOCATION + TERMINAL_BUFFER_SIZE) // 10 MB
#define BG32_MEM_LOCATION (DMA_MEM_LOCATION + (10 * MB))                   // 10 MB
#define BG8_MEM_LOCATION (BG32_MEM_LOCATION + (10 * MB))                   // 4 MB
#define VB_MEM_LOCATION (BG8_MEM_LOCATION + (4 * MB))                      // 16 MB