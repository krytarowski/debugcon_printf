/*	$NetBSD$	*/
/*-
 * Copyright (c) 2019 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Kamil Rytarowski.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */


#ifdef PRINTF
#include <stdio.h>
#define debugcon_printf printf
#else
#if defined(_KERNEL)
#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/module.h>
#endif
#include "debugcon_printf.h"
#endif

#if defined(_KERNEL)
static int
test(void)
#else
int
main(int argc, char **argv)
#endif
{
	char c;
	short s;
	int i;
	long l;
	long long ll;

	unsigned char uc;
	unsigned short us;
	unsigned int ui;
	unsigned long ul;
	unsigned long long ull;

	const char *str = "My string_12345";

	debugcon_printf("Hello world\n");
	debugcon_printf("'%s'\n", str);
	debugcon_printf("'%100s'\n", str);
	debugcon_printf("'%-100s'\n", str);
	debugcon_printf("\t'Tab.'\n");

	debugcon_printf("'%%'\n");

	c = 'X';
	s = 2;
	i = 3;
	l = 4;
	ll = 5;

	debugcon_printf("'%c'\n", c);
	debugcon_printf("'%hhd'\n", c);
	debugcon_printf("'%hd'\n", s);
	debugcon_printf("'%d'\n", i);
	debugcon_printf("'%ld'\n", l);
	debugcon_printf("'%lld'\n", ll);

	debugcon_printf("'%+hhd'\n", c);
	debugcon_printf("'%+hd'\n", s);
	debugcon_printf("'%+d'\n", i);
	debugcon_printf("'%+ld'\n", l);
	debugcon_printf("'%+lld'\n", ll);

	debugcon_printf("'% hhd'\n", c);
	debugcon_printf("'% hd'\n", s);
	debugcon_printf("'% d'\n", i);
	debugcon_printf("'% ld'\n", l);
	debugcon_printf("'% lld'\n", ll);

	debugcon_printf("'%hhx'\n", c);
	debugcon_printf("'%hx'\n", s);
	debugcon_printf("'%x'\n", i);
	debugcon_printf("'%lx'\n", l);
	debugcon_printf("'%llx'\n", ll);

	debugcon_printf("'%#hhx'\n", c);
	debugcon_printf("'%#hx'\n", s);
	debugcon_printf("'%#x'\n", i);
	debugcon_printf("'%#lx'\n", l);
	debugcon_printf("'%#llx'\n", ll);

	debugcon_printf("'%hho'\n", c);
	debugcon_printf("'%ho'\n", s);
	debugcon_printf("'%o'\n", i);
	debugcon_printf("'%lo'\n", l);
	debugcon_printf("'%llo'\n", ll);

	debugcon_printf("'%#hho'\n", c);
	debugcon_printf("'%#ho'\n", s);
	debugcon_printf("'%#o'\n", i);
	debugcon_printf("'%#lo'\n", l);
	debugcon_printf("'%#llo'\n", ll);

	uc = 1;
	us = 2;
	ui = 3;
	ul = 4;
	ull = 5;

	debugcon_printf("'%hhu'\n", uc);
	debugcon_printf("'%hu'\n", us);
	debugcon_printf("'%u'\n", ui);
	debugcon_printf("'%lu'\n", ul);
	debugcon_printf("'%llu'\n", ull);

	return 0;
}

#ifdef _KERNEL
MODULE(MODULE_CLASS_MISC, example, "debugcon_printf");

static int
example_modcmd(modcmd_t cmd, void *arg)
{

	switch (cmd) {
	case MODULE_CMD_INIT:
		test();
		return 0;
	case MODULE_CMD_FINI:
		return 0;
	default:
		return ENOTTY;
	}
}
#endif
