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

#include <windows.h>
#include <tchar.h>
#include <time.h>


#include <stddef.h>

#if defined(__GNUC__) || defined(__clang__)
#  define INLINE static inline __attribute__((always_inline))
#  define likely(x)   __builtin_expect(!!(x), 1)
#  define unlikely(x) __builtin_expect(!!(x), 0)
#else
#  define INLINE static inline
#  define likely(x)   (x)
#  define unlikely(x) (x)
#endif

#define snprintf _snprintf

struct timezone;

INLINE int gettimeofday(struct timeval *restrict tv, struct timezone *restrict tz);
INLINE void timeradd(const struct timeval *restrict a, const struct timeval *restrict b,
		     struct timeval *restrict result);
INLINE void timersub(const struct timeval *restrict a, const struct timeval *restrict b,
		     struct timeval *restrict result);

extern TCHAR *optarg;
extern int optind;

extern int getopt(int argc, TCHAR *argv[], TCHAR *optstring);

extern int count_processors(void);

#define PRSIZET "I"

static inline char *
strtok_r(char *strToken, const char *strDelimit, char **context) {
	return strtok_s(strToken, strDelimit, context);
}

