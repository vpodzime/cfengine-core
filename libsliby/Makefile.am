#
#  Copyright 2022 Northern.tech AS
#
#  This file is part of CFEngine 3 - written and maintained by Northern.tech AS.
#
#  This program is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by the
#  Free Software Foundation; version 3.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
#
# To the extent this program is licensed as part of the Enterprise
# versions of CFEngine, the applicable Commercial Open Source License
# (COSL) may apply to this file if you as a licensee so wish it. See
# included file COSL.txt.
#
projlib_LTLIBRARIES = libsliby.la

AM_CPPFLAGS = -I$(top_srcdir)/libntech/libutils \
	-I$(top_srcdir)/libcfnet \
	-I$(top_srcdir)/libpromises \
	$(PCRE_CPPFLAGS) \
	$(SYSTEMD_SOCKET_CPPFLAGS) \
	$(OPENSSL_CPPFLAGS)

libsliby_la_LIBADD = ../libpromises/libpromises.la
libsliby_la_LDFLAGS = -version-info 1:0:0

libsliby_la_SOURCES = sliby.c sliby.h
