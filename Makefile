#	$NetBSD$

#S?=	/usr/src/sys

KMOD=	debugcon_printf
SRCS=	debugcon_printf.c

.PHONY: test

test:
	${CC} -O0 -g -DTEST debugcon_printf.c test.c -o test_debugcon_printf
	${CC} -O0 -g -DTEST -DPRINTF debugcon_printf.c test.c -o test_real_printf
	./test_debugcon_printf > debugcon.txt
	./test_real_printf > real.txt
	diff -u debugcon.txt real.txt
	rm debugcon.txt real.txt

CLEANFILES+=	test

.include <bsd.kmodule.mk>
