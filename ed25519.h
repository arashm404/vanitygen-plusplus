#ifndef VANITYGEN_PLUSPLUS_ED25519_H
#define VANITYGEN_PLUSPLUS_ED25519_H

#include <stddef.h>    // for size_t
#include <stdatomic.h> // for _Atomic

typedef struct _vg_context_ed25519_s vg_context_ed25519_t;

#define ed25519_max_threads 1024

/* Application-level context. parameters and global pattern store */
struct _vg_context_ed25519_s _Alignas(64) {
    int			vc_addrtype;
    const char *restrict  vc_result_file;
    size_t      vc_numpairs;
    const char *restrict pattern;
    int         match_location; // 0: any, 1: begin, 2: end
    int			vc_verbose;
    _Atomic int	vc_halt;
    _Atomic int  vc_found_num;
    unsigned long vc_start_time;
    size_t      vc_thread_num;
    _Atomic unsigned long long vc_check_count[ed25519_max_threads];
};

int start_threads_ed25519(vg_context_ed25519_t *restrict vc_ed25519)
    __attribute__((nonnull(1)));

#endif //VANITYGEN_PLUSPLUS_ED25519_H
