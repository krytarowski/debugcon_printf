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

#include <sys/cdefs.h>

#ifdef TEST
__RCSID("$NetBSD$");
#include <stdio.h>
#else
__KERNEL_RCSID(0, "$NetBSD$");

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/module.h>
#endif

#include "debugcon_printf.h"

/*
 * Use builtin stdarg types to remove any dependency from external headers
 * in the debugcon_printf() implementation.
 */

#ifdef va_list
#undef va_list
#endif

#ifdef va_arg
#undef va_arg
#endif

#ifdef va_start
#undef va_start
#endif

#ifdef va_end
#undef va_end
#endif

#define va_list __builtin_va_list
#define va_arg __builtin_va_arg
#define va_start __builtin_va_start
#define va_end __builtin_va_end

static unsigned int iobase = 0xe9;

static void
internal_putchar(int c)
{
	int ch;

	ch = (unsigned char)c;
#ifdef TEST
	putchar(ch);
#else
#if defined(__x86_64__)
	__asm__ __volatile__("outb %%al, %%dx": : "a" (ch), "d" (iobase));
#else
#error PORT ME
#endif
#endif
}

static long
internal_strlen(char *s)
{
	char *p;

	for (p = s; *p; p++)
		continue;

	return (long)(p-s);
}

static void
internal_itoa(long long int i, int base)
{
	char buf[64]; /* The longest number string length */
	const char *digit = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	long long int temp;
	char *bp;

	bp = &buf[sizeof buf];
	*(--bp) = '\0';

	temp = (i >= 0) ? i : -i;
	do {
		*(--bp) = digit[temp % base];
	} while (temp /= base);

	if (i < 0)
		internal_putchar('-');
	while (*bp)
		internal_putchar(*(bp++));
}

static void
internal_utoa(unsigned long long int u, int base)
{
	char buf[64]; /* The longest number string length */
	const char *digit = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned long long int temp;
	char *bp;

	bp = &buf[sizeof buf];
	*(--bp) = '\0';

	temp = u;
	do {
		*(--bp) = digit[temp % base];
	} while (temp /= base);

	while (*bp)
		internal_putchar(*(bp++));
}

/* This function must not use any system functions, types or defines. */
void
debugcon_printf(const char *format, ...)
{
	va_list ap;
	const char *p, *p2;
	long long int c, d, x, o;
	unsigned long long int u;
	char *s;
	int flags;
	int shorter, longer;
	int sizet;

	int base;

	int hash;
	int leftadj;
	int plussign;
	int space;
	int zeropad;

	long len;

	long slen;

	long i;

	va_start(ap, format);
	for (p = format; *p; p++) {
		switch ((unsigned char)*p) {
		case '%':
			shorter = 0;
			longer = 0;
			sizet = 0;

			hash = 0;
			leftadj = 0;
			plussign = 0;
			space = 0;
			zeropad = 0;

			len = 0;
			p2 = p;

modifier:
			p++;
			switch ((unsigned char)*p) {
			case '\0':
				/* End of string? Print % and bail out */
				while (p2 != p && *p2)
					internal_putchar(*(p2++));
				break;
			case '#':
				++hash;
				goto modifier;
			case '-':
				++leftadj;
				goto modifier;
			case '+':
				++plussign;
				goto modifier;
			case ' ':
				++space;
				goto modifier;
			case '0':
				++zeropad;
				goto modifier;
			}

			/* Overrides */
			if (plussign)
				space = 0;
			if (leftadj)
				zeropad = 0;
			/* End of modifiers */
repeat:
			switch ((unsigned char)*p) {
			case '\0':
				/* End of string? Print % and bail out */
				while (p2 != p && *p2)
					internal_putchar(*(p2++));
				break;
			case '%':
				internal_putchar('%');
				break;
			case 's': /* string */
				s = va_arg(ap, char *);
				slen = internal_strlen(s);
				if (len > 0 && slen > len)
					slen = len;
				if (leftadj || slen == len) {
					for (i = 0; i < slen; i++)
						internal_putchar(s[i]);
					for (i = slen; i < len; i++)
						internal_putchar(' ');
				} else {
					for (i = slen; i < len; i++)
						internal_putchar(' ');
					for (i = 0; i < slen; i++)
						internal_putchar(s[i]);
				}
				break;
			case 'c': /* char */
				c = va_arg(ap, int); /* promoted */
				internal_putchar(c);
				break;
			case 'd': /* int */
				base = 10;
				goto itoa;
			case 'x': /* hex */
				base = 16;
				goto itoa;
			case 'o': /* oct */
				base = 8;
itoa:
				if (longer == 0 || shorter > 0)
					d = va_arg(ap, int);
				else if (longer == 1 || sizet == 1)
					d = va_arg(ap, long int);
				else if (longer == 2)
					d = va_arg(ap, long long int);
				if (hash && d != 0 && base != 10) {
					switch (base) {
					case 8:
						internal_putchar('0');
						break;
					case 16:
						internal_putchar('0');
						internal_putchar('x');
						break;
					}
				}
				if (plussign && d > 0)
					internal_putchar('+');
				if (space && d > 0)
					internal_putchar(' ');
				internal_itoa(d, base);
				break;
			case 'u': /* unsigned */
				if (longer == 0 || shorter > 0)
					u = va_arg(ap, unsigned int);
				else if (longer == 1)
					u = va_arg(ap, unsigned long int);
				else if (longer == 2 || sizet == 1)
					u = va_arg(ap, unsigned long long int);
				internal_utoa(u, 10);
				break;
			case 'h':
				++shorter;
				if (longer > 0 || shorter > 2 || sizet > 0) {
					while (p2 != p)
						internal_putchar(*(p2++));
					break;
				}
				++p;
				goto repeat;
			case 'l':
				++longer;
				if (longer > 2 || shorter > 0 || sizet > 0) {
					while (p2 != p)
						internal_putchar(*(p2++));
					break;
				}
				++p;
				goto repeat;
			case 'z': /* assume size_t == ulong */
				++sizet;
				if (longer > 0 || shorter > 0 || sizet > 1) {
					while (p2 != p)
						internal_putchar(*(p2++));
					break;
				}
				++p;
				goto repeat;
			case '0' ... '9':
				if (len > 0) {
					while (p2 != p)
						internal_putchar(*(p2++));
					break;
				}
				for (;;) {
					switch ((unsigned char)*p) {
					case '0' ... '9':
						len *= 10;
						len += (unsigned char)*p - '0';
						++p;
						break;
					default:
						goto parsed_len;
					}
				}
parsed_len:
				goto repeat;
			case '*': case '.':
				/* Not supported */
				/* FALLTHROUGH */
			default:
				/* Unknown/unsupported modifier, print it out */
				while (p2 != p)
					internal_putchar(*(p2++));
			}
			break;
		default:
			internal_putchar(*p);
		}
	}
	va_end(ap);
}

#ifndef TEST
MODULE(MODULE_CLASS_MISC, debugcon_printf, NULL);

static int
debugcon_printf_modcmd(modcmd_t cmd, void *arg)
{
	prop_dictionary_t dict;
	prop_object_t obj;

	switch (cmd) {
	case MODULE_CMD_INIT:
		dict = (prop_dictionary_t)arg;

		if (dict == NULL)
			goto no_iobase;

		obj = prop_dictionary_get(dict, "iobase");
		if (obj == NULL)
			goto no_iobase;

		if (prop_object_type(obj) != PROP_TYPE_NUMBER)
			goto no_iobase;

		iobase = (unsigned int)prop_number_unsigned_integer_value(obj);

no_iobase:
		printf("debugcon_printf initialized with iobase=%#x\n", iobase);
		return 0;
	case MODULE_CMD_FINI:
		return 0;
	default:
		return ENOTTY;
	}
}
#endif
