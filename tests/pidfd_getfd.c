/*
 * Check decoding of pidfd_getfd syscall.
 *
 * Copyright (c) 2019 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2020 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include "scno.h"

#ifdef __NR_pidfd_getfd

# include <stdio.h>
# include <unistd.h>

# ifndef PIDFD_PATH
#  define PIDFD_PATH ""
# endif
# ifndef FD0_PATH
#  define FD0_PATH ""
# endif

static const char *errstr;

static long
k_pidfd_getfd(const unsigned int pid, const unsigned int fd,
	      const unsigned int flags)
{
	const kernel_ulong_t fill = (kernel_ulong_t) 0xdefaced00000000ULL;
	const kernel_ulong_t bad = (kernel_ulong_t) 0xbadc0dedbadc0dedULL;
	const kernel_ulong_t arg1 = fill | pid;
	const kernel_ulong_t arg2 = fill | fd;
	const kernel_ulong_t arg3 = fill | flags;
	const long rc = syscall(__NR_pidfd_getfd,
				arg1, arg2, arg3, bad, bad, bad);
	errstr = sprintrc(rc);
	return rc;
}

int
main(void)
{
	long rc;

	rc = k_pidfd_getfd(-1U, -1U, 0);
	printf("pidfd_getfd(-1, -1, 0) = %s\n", errstr);

	rc = k_pidfd_getfd(0, 0, 0xbadc0ded);
	printf("pidfd_getfd(0" FD0_PATH ", 0, 0xbadc0ded) = %s\n", errstr);

	int pid = getpid();
	int pidfd = syscall(__NR_pidfd_open, pid, 0);
	rc = k_pidfd_getfd(pidfd, 0, 0);
	printf("pidfd_getfd(%d%s, 0, 0) = %s%s\n",
	       pidfd, pidfd >= 0 ? PIDFD_PATH : "",
	       errstr, rc >= 0 ? FD0_PATH : "");

	puts("+++ exited with 0 +++");
	return 0;
}

#else

SKIP_MAIN_UNDEFINED("__NR_pidfd_getfd");

#endif
