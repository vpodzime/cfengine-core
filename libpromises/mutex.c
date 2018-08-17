/*
   Copyright 2018 Northern.tech AS

   This file is part of CFEngine 3 - written and maintained by Northern.tech AS.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; version 3.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

  To the extent this program is licensed as part of the Enterprise
  versions of CFEngine, the applicable Commercial Open Source License
  (COSL) may apply to this file if you as a licensee so wish it. See
  included file COSL.txt.
*/

#include <mutex.h>

#include <logging.h>                                            /* Log */


static pthread_mutex_t MUTEXES[] = /* GLOBAL_T */
{
    PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP,
    PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP,
    PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP,
    PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP,
    PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP,
};

pthread_mutex_t *cft_lock = &MUTEXES[0]; /* GLOBAL_T */
pthread_mutex_t *cft_count = &MUTEXES[1]; /* GLOBAL_T */
pthread_mutex_t *cft_getaddr = &MUTEXES[2]; /* GLOBAL_T */
pthread_mutex_t *cft_server_children = &MUTEXES[3]; /* GLOBAL_T */
pthread_mutex_t *cft_server_filter = &MUTEXES[4]; /* GLOBAL_T */

unsigned long cft_lock_cnt            = 0;  /* microseconds */
unsigned long cft_count_cnt           = 0;
unsigned long cft_getaddr_cnt         = 0;
unsigned long cft_server_children_cnt = 0;
unsigned long cft_server_filter_cnt   = 0;

int __ThreadLock(pthread_mutex_t *mutex,
                 const char *funcname, const char *filename, int lineno)

{
    int result = pthread_mutex_trylock(mutex);
    if (result == 0)
    {
        return true;
    }
    else
    {
        if (result != EBUSY)
        {
            /* Log() is blocking on mutexes itself inside malloc(), so maybe not
             * the best idea here. */
            Log(LOG_LEVEL_ERR,
                "Locking failure at %s:%d function %s! (pthread_mutex_lock: %s)",
                filename, lineno, funcname, GetErrorStrFromCode(result));
            return false;
        }
        else
        {
            struct timespec start;
            struct timespec done;
            clock_gettime(CLOCK_MONOTONIC_RAW, &start);
            result = pthread_mutex_lock(mutex);
            clock_gettime(CLOCK_MONOTONIC_RAW, &done);

            if (result != 0)
            {
                Log(LOG_LEVEL_ERR,
                    "Locking failure at %s:%d function %s! (pthread_mutex_lock: %s)",
                    filename, lineno, funcname, GetErrorStrFromCode(result));
                return false;
            }

            unsigned long diff = (done.tv_sec - start.tv_sec) * 1000000; /* microseconds */
            long nsec_diff = done.tv_nsec - start.tv_nsec;               /* nanoseconds  */
            diff += nsec_diff / 1000;                                    /* microseconds */

            if (mutex == cft_lock)
            {
                cft_lock_cnt += diff;
            }
            else if (mutex == cft_count)
            {
                cft_count_cnt += diff;
            }
            else if (mutex == cft_getaddr)
            {
                cft_getaddr_cnt += diff;
            }
            else if (mutex == cft_server_children)
            {
                cft_server_children_cnt += diff;
            }
            else if (mutex == cft_server_filter)
            {
                cft_server_filter_cnt += diff;
            }
        }
    }

    return true;
}

int __ThreadUnlock(pthread_mutex_t *mutex,
                 const char *funcname, const char *filename, int lineno)
{
    int result = pthread_mutex_unlock(mutex);

    if (result != 0)
    {
        /* Log() is blocking on mutexes itself inside malloc(), so maybe not
         * the best idea here. */
        Log(LOG_LEVEL_ERR,
            "Locking failure at %s:%d function %s! (pthread_mutex_unlock: %s)",
            filename, lineno, funcname, GetErrorStrFromCode(result));
        return false;
    }

    return true;
}

void DumpLockDiagnostics(void)
{
    Log(LOG_LEVEL_WARNING, "cft_lock: %lu", cft_lock_cnt);
    Log(LOG_LEVEL_WARNING, "cft_count: %lu", cft_count_cnt);
    Log(LOG_LEVEL_WARNING, "cft_getaddr: %lu", cft_getaddr_cnt);
    Log(LOG_LEVEL_WARNING, "cft_server_children: %lu", cft_server_children_cnt);
    Log(LOG_LEVEL_WARNING, "cft_server_filter: %lu", cft_server_filter_cnt);
}
