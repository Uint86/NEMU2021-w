#include <stdio.h>
#include <stdint.h>
#include "FLOAT.h"

extern char _vfprintf_internal;
extern char _fpmaxtostr;
extern int __stdio_fwrite(char *buf, int len, FILE *stream);

__attribute__((used)) static int format_FLOAT(FILE *stream, FLOAT f) {
	/* TODO: Format a FLOAT argument `f' and write the formating
	 * result to `stream'. Keep the precision of the formating
	 * result with 6 by truncating. For example:
	 *              f          result
	 *         0x00010000    "1.000000"
	 *         0x00013333    "1.199996"
	 */

	char buf[80];
	
	uint32_t magnitude =
		f < 0 ? -(uint32_t)f : (uint32_t)f;
	uint32_t integer = magnitude >> 16;
	uint32_t fraction =
		((magnitude & 0xffffu) * 15625u) >> 10;

	int len = sprintf(
		buf,
		"%s%u.%06u",
		f < 0 ? "-" : "",
		integer,
		fraction
	);

	return __stdio_fwrite(buf, len, stream);
}

static void modify_vfprintf() {
		unsigned char *p = (unsigned char *)&_vfprintf_internal;
	int i;
	int call_pos = -1;

	/*
	 * 查找机器码：
	 *
	 *     e8 xx xx xx xx    call _fpmaxtostr
	 *
	 * rel32 的目标地址计算方式为：
	 *
	 *     下一条指令地址 + rel32
	 */
	for (i = 0; i < 4096; i++) {
		if (p[i] == 0xe8) {
			int32_t displacement = *(int32_t *)(p + i + 1);
			unsigned char *target =
				p + i + 5 + displacement;

			if (target == (unsigned char *)&_fpmaxtostr) {
				call_pos = i;

				*(int32_t *)(p + i + 1) =
					(int32_t)(
						(unsigned char *)&format_FLOAT -
						(p + i + 5)
					);
				break;
			}
		}
	}

	nemu_assert(call_pos >= 0);

	/*
	 * 只检查 call 前面的 64 字节，避免修改其它分支中
	 * 恰好相同的机器码。
	 */
	int begin = call_pos > 64 ? call_pos - 64 : 0;
	int load_count = 0;
	int store_count = 0;

	for (i = begin; i < call_pos; i++) {
		if (p[i] == 0xdb && p[i + 1] == 0x2a) {
			/* fldt (%edx) -> movl (%edx), %ecx */
			p[i] = 0x8b;
			p[i + 1] = 0x0a;
			load_count++;
		}
		else if (p[i] == 0xdd && p[i + 1] == 0x02) {
			/* fldl (%edx) -> movl (%edx), %ecx */
			p[i] = 0x8b;
			p[i + 1] = 0x0a;
			load_count++;
		}
		else if (
			p[i] == 0xdb &&
			p[i + 1] == 0x3c &&
			p[i + 2] == 0x24
		) {
			/* fstpt (%esp) -> movl %ecx, (%esp) */
			p[i] = 0x89;
			p[i + 1] = 0x0c;
			p[i + 2] = 0x24;
			store_count++;
		}
	}

	nemu_assert(load_count == 2);
	nemu_assert(store_count == 1);
	/* TODO: Implement this function to hijack the formating of "%f"
	 * argument during the execution of `_vfprintf_internal'. Below
	 * is the code section in _vfprintf_internal() relative to the
	 * hijack.
	 */

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = _fpmaxtostr(stream,
				(__fpmax_t)
				(PRINT_INFO_FLAG_VAL(&(ppfs->info),is_long_double)
				 ? *(long double *) *argptr
				 : (long double) (* (double *) *argptr)),
				&ppfs->info, FP_OUT );
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

	/* You should modify the run-time binary to let the code above
	 * call `format_FLOAT' defined in this source file, instead of
	 * `_fpmaxtostr'. When this function returns, the action of the
	 * code above should do the following:
	 */

#if 0
	else if (ppfs->conv_num <= CONV_A) {  /* floating point */
		ssize_t nf;
		nf = format_FLOAT(stream, *(FLOAT *) *argptr);
		if (nf < 0) {
			return -1;
		}
		*count += nf;

		return 0;
	} else if (ppfs->conv_num <= CONV_S) {  /* wide char or string */
#endif

}

static void modify_ppfs_setargs() {
		extern char _ppfs_setargs;

	unsigned char *p = (unsigned char *)&_ppfs_setargs;
	int i;
	int fldl_pos = -1;
	int long_long_pos = -1;

	/* 查找 PA_DOUBLE 分支中的 fldl (%edx)。 */
	for (i = 0; i < 512; i++) {
		if (p[i] == 0xdd && p[i + 1] == 0x02) {
			fldl_pos = i;
			break;
		}
	}

	/*
	 * 查找 64 位整数读取代码：
	 *
	 *     movl  (%edx), %edi
	 *     movl 4(%edx), %ebp
	 *
	 * 机器码：
	 *
	 *     8b 3a 8b 6a 04
	 */
	for (i = 0; i < 512; i++) {
		if (
			p[i] == 0x8b &&
			p[i + 1] == 0x3a &&
			p[i + 2] == 0x8b &&
			p[i + 3] == 0x6a &&
			p[i + 4] == 0x04
		) {
			long_long_pos = i;
			break;
		}
	}

	nemu_assert(fldl_pos >= 0);
	nemu_assert(long_long_pos >= 0);

	int displacement =
		long_long_pos - (fldl_pos + 2);

	nemu_assert(displacement >= -128);
	nemu_assert(displacement <= 127);

	/*
	 * eb xx 是两字节短跳转：
	 *
	 *     jmp 64位整数读取分支
	 */
	p[fldl_pos] = 0xeb;
	p[fldl_pos + 1] = (unsigned char)displacement;

	/* 原浮点读取分支剩下的字节改为 NOP。 */
	for (i = fldl_pos + 2; i <= fldl_pos + 8; i++) {
		p[i] = 0x90;
	}
	
	/* TODO: Implement this function to modify the action of preparing
	 * "%f" arguments for _vfprintf_internal() in _ppfs_setargs().
	 * Below is the code section in _vfprintf_internal() relative to
	 * the modification.
	 */

#if 0
	enum {                          /* C type: */
		PA_INT,                       /* int */
		PA_CHAR,                      /* int, cast to char */
		PA_WCHAR,                     /* wide char */
		PA_STRING,                    /* const char *, a '\0'-terminated string */
		PA_WSTRING,                   /* const wchar_t *, wide character string */
		PA_POINTER,                   /* void * */
		PA_FLOAT,                     /* float */
		PA_DOUBLE,                    /* double */
		__PA_NOARG,                   /* non-glibc -- signals non-arg width or prec */
		PA_LAST
	};

	/* Flag bits that can be set in a type returned by `parse_printf_format'.  */
	/* WARNING -- These differ in value from what glibc uses. */
#define PA_FLAG_MASK		(0xff00)
#define __PA_FLAG_CHAR		(0x0100) /* non-gnu -- to deal with hh */
#define PA_FLAG_SHORT		(0x0200)
#define PA_FLAG_LONG		(0x0400)
#define PA_FLAG_LONG_LONG	(0x0800)
#define PA_FLAG_LONG_DOUBLE	PA_FLAG_LONG_LONG
#define PA_FLAG_PTR		(0x1000) /* TODO -- make dynamic??? */

	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			case (PA_INT|PA_FLAG_LONG):
				GET_VA_ARG(p,ul,unsigned long,ppfs->arg);
				break;
			case PA_CHAR:	/* TODO - be careful */
				/* ... users could use above and really want below!! */
			case (PA_INT|__PA_FLAG_CHAR):/* TODO -- translate this!!! */
			case (PA_INT|PA_FLAG_SHORT):
			case PA_INT:
				GET_VA_ARG(p,u,unsigned int,ppfs->arg);
				break;
			case PA_WCHAR:	/* TODO -- assume int? */
				/* we're assuming wchar_t is at least an int */
				GET_VA_ARG(p,wc,wchar_t,ppfs->arg);
				break;
				/* PA_FLOAT */
			case PA_DOUBLE:
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			case (PA_DOUBLE|PA_FLAG_LONG_DOUBLE):
				GET_VA_ARG(p,ld,long double,ppfs->arg);
				break;
			default:
				/* TODO -- really need to ensure this can't happen */
				assert(ppfs->argtype[i-1] & PA_FLAG_PTR);
			case PA_POINTER:
			case PA_STRING:
			case PA_WSTRING:
				GET_VA_ARG(p,p,void *,ppfs->arg);
				break;
			case __PA_NOARG:
				continue;
		}
		++p;
	}
#endif

	/* You should modify the run-time binary to let the `PA_DOUBLE'
	 * branch execute the code in the `(PA_INT|PA_FLAG_LONG_LONG)'
	 * branch. Comparing to the original `PA_DOUBLE' branch, the
	 * target branch will also prepare a 64-bit argument, without
	 * introducing floating point instructions. When this function
	 * returns, the action of the code above should do the following:
	 */

#if 0
	while (i < ppfs->num_data_args) {
		switch(ppfs->argtype[i++]) {
			case (PA_INT|PA_FLAG_LONG_LONG):
			here:
				GET_VA_ARG(p,ull,unsigned long long,ppfs->arg);
				break;
			// ......
				/* PA_FLOAT */
			case PA_DOUBLE:
				goto here;
				GET_VA_ARG(p,d,double,ppfs->arg);
				break;
			// ......
		}
		++p;
	}
#endif

}

void init_FLOAT_vfprintf() {
	modify_vfprintf();
	modify_ppfs_setargs();
}
