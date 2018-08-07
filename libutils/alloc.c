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

#define ALLOC_IMPL
#include <platform.h>
#include <alloc.h>

#define CheckResult(ptr, check_result)                          \
    if (((ptr) == NULL) && (check_result))                      \
    {                                                           \
        fputs(__func__, stderr);                                \
        fputs("CRITICAL: Unable to allocate memory\n", stderr); \
        exit(255);                                              \
    }                                                           \


void *xmalloc(size_t size)
{
    void *ptr = malloc(size);
    CheckResult(ptr, size != 0);
    return ptr;
}

void *xcalloc(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);
    CheckResult(ptr, (nmemb != 0) && (size != 0));
    return ptr;
}

void *xrealloc(void *orig_ptr, size_t size)
{
    void *ptr = realloc(orig_ptr, size);
    CheckResult(ptr, size != 0);
    return ptr;
}

char *xstrdup(const char *str)
{
    void *ptr = strdup(str);
    CheckResult(ptr, true);
    return ptr;
}

char *xstrndup(const char *str, size_t n)
{
    void *ptr = strndup(str, n);
    CheckResult(ptr, true);
    return ptr;
}

void *xmemdup(const void *data, size_t size)
{
    void *ptr = memdup(data, size);
    CheckResult(ptr, size != 0);
    return ptr;
}

int xasprintf(char **strp, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    int res = xvasprintf(strp, fmt, ap);

    va_end(ap);
    return res;
}

int xvasprintf(char **strp, const char *fmt, va_list ap)
{
    int res = vasprintf(strp, fmt, ap);

    CheckResult(res == -1 ? NULL : *strp, true);
    return res;
}
