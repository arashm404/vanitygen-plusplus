/*
 * Vanitygen, vanity bitcoin address generator
 * Copyright (C) 2011 <samr7@cs.washington.edu>
 *
 * Vanitygen is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version. 
 *
 * Vanitygen is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with Vanitygen.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <openssl/bn.h>
#include <openssl/ec.h>

#include <pthread.h>

#ifdef _WIN32
#include "winglue.h"
#else
#define INLINE inline
#define PRSIZET "z"
#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

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
#endif

#define VANITYGEN_VERSION "PLUS PLUS v2.01"

#define ADDR_TYPE_ETH -1
#define PRIV_TYPE_ETH -1

#define ADDR_TYPE_XLM -2
#define PRIV_TYPE_XLM -2

#define ADDR_TYPE_ATOM -3
#define PRIV_TYPE_ATOM -3

typedef struct _vg_context_s vg_context_t;

struct _vg_exec_context_s;
typedef struct _vg_exec_context_s vg_exec_context_t;

typedef void *(*vg_exec_context_threadfunc_t)(vg_exec_context_t *);

/* Context of one pattern-matching unit within the process */
struct _vg_exec_context_s _Alignas(64) {
	vg_context_t			*vxc_vc;
	_Atomic BN_CTX			*vxc_bnctx;
	EC_KEY				*vxc_key;
	int				vxc_delta;
	unsigned char			vxc_binres[28];
	BIGNUM *restrict			vxc_bntarg;
	BIGNUM *restrict			vxc_bnbase;
	BIGNUM *restrict			vxc_bntmp;
	BIGNUM *restrict			vxc_bntmp2;

	vg_exec_context_threadfunc_t	vxc_threadfunc;
	pthread_t			vxc_pthread;
	int				vxc_thread_active;

	/* Thread synchronization */
	struct _vg_exec_context_s	*vxc_next;
	int				vxc_lockmode;
	int				vxc_stop;
};


typedef void (*vg_free_func_t)(vg_context_t *);
typedef int (*vg_add_pattern_func_t)(vg_context_t *,
				     const char ** const patterns,
				     int npatterns);
typedef void (*vg_clear_all_patterns_func_t)(vg_context_t *);
typedef int (*vg_test_func_t)(vg_exec_context_t *);
typedef int (*vg_addr_sort_func_t)(vg_context_t *vcp, void *buf);
typedef void (*vg_output_error_func_t)(vg_context_t *vcp, const char *info);
typedef void (*vg_output_match_func_t)(vg_context_t *vcp, EC_KEY *pkey,
				       const char *pattern);
typedef void (*vg_output_timing_func_t)(vg_context_t *vcp, double count,
					unsigned long long rate,
					unsigned long long total);

enum vg_format {
	VCF_PUBKEY,
	VCF_SCRIPT,
	VCF_CONTRACT, // VCF_CONTRACT only valid for ETH
	VCF_P2WPKH,
	VCF_P2TR
};

/* Application-level context, incl. parameters and global pattern store */
struct _vg_context_s {
	int			vc_compressed;
	int			vc_addrtype;
	int			vc_privtype;
	unsigned long		vc_npatterns;
	unsigned long		vc_npatterns_start;
	unsigned long long	vc_found;
	int			vc_pattern_generation;
	double			vc_chance;
	const char		*vc_result_file;
	const char		*vc_key_protect_pass;
	int			vc_remove_on_match;
	int        		vc_numpairs;
	int			vc_only_one;
	int        		vc_csv;
	int			vc_verbose;
	enum vg_format		vc_format;
	int			vc_pubkeytype;
	EC_POINT		*vc_pubkey_base;
	char			vc_privkey_prefix[32];
	int			vc_privkey_prefix_nbits;
	int			vc_halt;

	vg_exec_context_t	*vc_threads;
	int			vc_thread_excl;

	/* Internal methods */
	vg_free_func_t			vc_free;
	vg_add_pattern_func_t		vc_add_patterns;
	vg_clear_all_patterns_func_t	vc_clear_all_patterns;
	vg_test_func_t			vc_test;
	vg_addr_sort_func_t		vc_addr_sort;

	/* Performance related members */
	unsigned long long		vc_timing_total;
	unsigned long long		vc_timing_prevfound;
	unsigned long long		vc_timing_sincelast;
	struct _timing_info_s		*vc_timing_head;

	/* External methods */
	vg_output_error_func_t		vc_output_error;
	vg_output_match_func_t		vc_output_match;
	vg_output_timing_func_t		vc_output_timing;
};


/* Base context methods */
extern void vg_context_free(vg_context_t *restrict vcp) __attribute__((nonnull(1)));
extern int vg_context_add_patterns(vg_context_t *restrict vcp,
				   const char *const *restrict patterns, size_t npatterns) __attribute__((nonnull(1,2))) __attribute__((warn_unused_result));
extern void vg_context_clear_all_patterns(vg_context_t *vcp);
extern int vg_context_start_threads(vg_context_t *vcp);
extern void vg_context_stop_threads(vg_context_t *vcp);
extern void vg_context_wait_for_completion(vg_context_t *vcp);

/* Prefix context methods */
extern vg_context_t *restrict vg_prefix_context_new(int addrtype, int privtype,
					   int caseinsensitive) __attribute__((warn_unused_result));
extern void vg_prefix_context_set_case_insensitive(vg_context_t *vcp,
						   int caseinsensitive);
extern int vg_prefix_context_get_case_insensitive(vg_context_t *vcp);
extern double vg_prefix_get_difficulty(int addrtype, const char *pattern);

/* Regex context methods */
extern vg_context_t *vg_regex_context_new(int addrtype, int privtype);

/* Utility functions */
extern int vg_output_timing(vg_context_t *vcp, int cycle, struct timeval *last);
extern void vg_output_match_console(vg_context_t *vcp, EC_KEY *pkey,
				    const char *pattern);
extern void vg_output_timing_console(vg_context_t *vcp, double count,
				     unsigned long long rate,
				     unsigned long long total);



/* Internal vg_context methods */
extern int vg_context_addr_sort(vg_context_t *vcp, void *buf);
extern void vg_context_thread_exit(vg_context_t *vcp);

/* Internal Init/cleanup for common execution context */
extern int vg_exec_context_init(vg_context_t *vcp, vg_exec_context_t *vxcp);
extern void vg_exec_context_del(vg_exec_context_t *vxcp);
extern void vg_exec_context_consolidate_key(vg_exec_context_t *vxcp);
extern void vg_exec_context_calc_address(vg_exec_context_t *vxcp);
extern EC_KEY *vg_exec_context_new_key(void);

/* Internal execution context lock handling functions */
extern void vg_exec_context_downgrade_lock(vg_exec_context_t *vxcp);
extern int vg_exec_context_upgrade_lock(vg_exec_context_t *vxcp);
extern void vg_exec_context_yield(vg_exec_context_t *vxcp);
