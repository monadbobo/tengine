
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


#if (( __i386__ || __amd64__ ) && ( __GNUC__ || __INTEL_COMPILER ))


#define NGX_CACHE_LVL_1_DATA            1
#define NGX_CACHE_LVL_2                 2
#define NGX_CACHE_LVL_3                 3
#define NGX_CACHE_PREFETCHING           4


typedef struct ngx_cache_table {
    u_char            descriptor;
    u_char            type;
    ngx_uint_t        size;
} ngx_cache_table_t;


static ngx_inline void ngx_cpuid(uint32_t i, uint32_t *buf);


static ngx_cache_table_t  cache_table[] = {
    { 0x0a, NGX_CACHE_LVL_1_DATA, 32 },  /* 32 byte line size */
    { 0x0c, NGX_CACHE_LVL_1_DATA, 32 },  /* 32 byte line size */
    { 0x0d, NGX_CACHE_LVL_1_DATA, 64 },  /* 64 byte line size */
    { 0x0e, NGX_CACHE_LVL_1_DATA, 64 },  /* 64 byte line size */
    { 0x21, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x22, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x23, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x25, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x29, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x2c, NGX_CACHE_LVL_1_DATA, 64 },  /* 64 byte line size */
    { 0x39, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x3a, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x3b, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x3c, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x3d, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x3e, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x3f, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x41, NGX_CACHE_LVL_2, 32 },       /* 32 byte line size */
    { 0x42, NGX_CACHE_LVL_2, 32 },       /* 32 byte line size */
    { 0x43, NGX_CACHE_LVL_2, 32 },       /* 32 byte line size */
    { 0x44, NGX_CACHE_LVL_2, 32 },       /* 32 byte line size */
    { 0x45, NGX_CACHE_LVL_2, 32 },       /* 32 byte line size */
    { 0x46, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x47, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x48, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x49, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x4a, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x4b, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x4c, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x4d, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0x4e, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x60, NGX_CACHE_LVL_1_DATA, 64 },  /* 64 byte line size */
    { 0x66, NGX_CACHE_LVL_1_DATA, 64 },  /* 64 byte line size */
    { 0x67, NGX_CACHE_LVL_1_DATA, 64 },  /* 64 byte line size */
    { 0x68, NGX_CACHE_LVL_1_DATA, 64 },  /* 64 byte line size */
    { 0x78, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x79, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x7a, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x7b, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x7c, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x7d, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x7f, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x80, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x82, NGX_CACHE_LVL_2, 32 },       /* 32 byte line size */
    { 0x83, NGX_CACHE_LVL_2, 32 },       /* 32 byte line size */
    { 0x84, NGX_CACHE_LVL_2, 32 },       /* 32 byte line size */
    { 0x85, NGX_CACHE_LVL_2, 32 },       /* 32 byte line size */
    { 0x86, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0x87, NGX_CACHE_LVL_2, 64 },       /* 64 byte line size */
    { 0xd0, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xd1, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xd2, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xd6, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xd7, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xd8, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xdc, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xdd, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xde, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xe2, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xe3, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xe4, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xea, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xeb, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xec, NGX_CACHE_LVL_3, 64 },       /* 64 byte line size */
    { 0xf0, NGX_CACHE_PREFETCHING, 64 },  /* 64-byte prefetching */
    { 0xf1, NGX_CACHE_PREFETCHING, 128 }, /* 128-byte prefetching */
    { 0x00, 0, 0}
};


#if ( __i386__ )

static ngx_inline void
ngx_cpuid(uint32_t i, uint32_t *buf)
{

    /*
     * we could not use %ebx as output parameter if gcc builds PIC,
     * and we could not save %ebx on stack, because %esp is used,
     * when the -fomit-frame-pointer optimization is specified.
     */

    __asm__ (

    "    mov    %%ebx, %%esi;  "

    "    cpuid;                "
    "    mov    %%eax, (%1);   "
    "    mov    %%ebx, 4(%1);  "
    "    mov    %%edx, 8(%1);  "
    "    mov    %%ecx, 12(%1); "

    "    mov    %%esi, %%ebx;  "

    : : "a" (i), "D" (buf) : "ecx", "edx", "esi", "memory" );
}


#else /* __amd64__ */


static ngx_inline void
ngx_cpuid(uint32_t i, uint32_t *buf)
{
    uint32_t  eax, ebx, ecx, edx;

    __asm__ (

        "cpuid"

    : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) : "a" (i) );

    buf[0] = eax;
    buf[1] = ebx;
    buf[2] = edx;
    buf[3] = ecx;
}


#endif


static ngx_inline
uint32_t ngx_cpuid_eax(uint32_t op)
{
    uint32_t   cpu[4];

    ngx_cpuid(op, cpu);

    return cpu[0];
}


/* auto detect the L2 cache line size of modern and widespread CPUs */

void
ngx_cpuinfo(void)
{
    u_char    *vendor, *dp, des;
    uint32_t   vbuf[5], cache[4], n;
    ngx_uint_t i, j, k, l1, l2, l3, prefetch;

    vbuf[0] = 0;
    vbuf[1] = 0;
    vbuf[2] = 0;
    vbuf[3] = 0;
    vbuf[4] = 0;

    l1 = 0;
    l2 = 0;
    l3 = 0;
    prefetch = 0;

    dp = (u_char *) cache;

    ngx_cpuid(0, vbuf);

    vendor = (u_char *) &vbuf[1];

    if (vbuf[0] == 0) {
        return;
    }

    if (ngx_strcmp(vendor, "GenuineIntel") == 0) {

        n = ngx_cpuid_eax(2) & 0xFF;

        for (i = 0 ; i < n ; i++) {
            ngx_cpuid(2, cache);

            for (j = 0; j < 3; j++) {
                if (cache[j] & (1 << 31)) {
                    cache[j] = 0;
                }
            }

            for (j = 1; j < 16; j++) {
                des = dp[j];
                k = 0;

                while (cache_table[k].descriptor != 0) {
                    if (cache_table[k].descriptor == des) {

                        switch (cache_table[k].type) {

                        case NGX_CACHE_LVL_1_DATA:
                            l1 = cache_table[k].size;
                            break;

                        case NGX_CACHE_LVL_2:
                            l2 = cache_table[k].size;
                            break;

                        case NGX_CACHE_LVL_3:
                            l3 = cache_table[k].size;
                            break;

                        case NGX_CACHE_PREFETCHING:
                            prefetch = cache_table[k].size;
                            break;
                        }

                        break;
                    }

                    k++;
                }
            }
        }

        ngx_cacheline_size = ngx_max(l1, l2);
        ngx_cacheline_size = ngx_max(l3, ngx_cacheline_size);
        ngx_cacheline_size = ngx_max(prefetch, ngx_cacheline_size);

    } else if (ngx_strcmp(vendor, "AuthenticAMD") == 0) {
        ngx_cacheline_size = 64;
    }
}

#else


void
ngx_cpuinfo(void)
{
}


#endif
