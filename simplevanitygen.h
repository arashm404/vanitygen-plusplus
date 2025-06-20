#pragma once

#include "pattern.h"

#include <stddef.h>    // for size_t
#include <stdatomic.h> // for _Atomic

#if defined(__GNUC__) || defined(__clang__)
#  define INLINE static inline __attribute__((always_inline))
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
#else
#  define INLINE static inline
#  define likely(x)   (x)
#  define unlikely(x) (x)
#endif

typedef struct _vg_context_simplevanitygen_s vg_context_simplevanitygen_t;

#define simplevanitygen_max_threads 1024

/* Application-level context. parameters and global pattern store */
struct _vg_context_simplevanitygen_s _Alignas(64) {
    int vc_addrtype;
    int vc_privtype;
    const char *restrict vc_coin;
    const char *restrict vc_hrp; // Human-Readable Part of bech32
    const char *restrict vc_result_file;
    size_t vc_numpairs;
    const char *restrict pattern;
    size_t match_location; // 0: any, 1: begin, 2: end
    int vc_verbose;
    enum vg_format vc_format;
    _Atomic int vc_halt;
    _Atomic int vc_found_num;
    unsigned long vc_start_time;
    size_t vc_thread_num;
    _Atomic unsigned long long vc_check_count[simplevanitygen_max_threads];
};

int start_threads_simplevanitygen(vg_context_simplevanitygen_t *restrict ctx)
    __attribute__((nonnull(1))) __attribute__((warn_unused_result));
