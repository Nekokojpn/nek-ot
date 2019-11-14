	.text
	.def	 @feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"io.cpp"
	.def	 "?input@@YAHXZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?input@@YAHXZ"         # -- Begin function ?input@@YAHXZ
	.p2align	4, 0x90
"?input@@YAHXZ":                        # @"?input@@YAHXZ"
.seh_proc "?input@@YAHXZ"
# %bb.0:
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	leaq	"??_C@_02DPKJAMEF@?$CFd?$AA@"(%rip), %rcx
	leaq	36(%rsp), %rdx
	callq	scanf_s
	movl	36(%rsp), %r8d
	movl	%eax, 32(%rsp)          # 4-byte Spill
	movl	%r8d, %eax
	addq	$40, %rsp
	retq
	.seh_handlerdata
	.text
	.seh_endproc
                                        # -- End function
	.def	 scanf_s;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,scanf_s
	.globl	scanf_s                 # -- Begin function scanf_s
	.p2align	4, 0x90
scanf_s:                                # @scanf_s
.seh_proc scanf_s
# %bb.0:
	subq	$88, %rsp
	.seh_stackalloc 88
	.seh_endprologue
	movq	%r9, 120(%rsp)
	movq	%r8, 112(%rsp)
	movq	%rdx, 104(%rsp)
	movq	%rcx, 80(%rsp)
	leaq	104(%rsp), %rcx
	movq	%rcx, 64(%rsp)
	movq	64(%rsp), %r9
	movq	80(%rsp), %rdx
	xorl	%eax, %eax
	movl	%eax, %ecx
	movq	%r9, 56(%rsp)           # 8-byte Spill
	movq	%rdx, 48(%rsp)          # 8-byte Spill
	movl	%eax, 44(%rsp)          # 4-byte Spill
	callq	__acrt_iob_func
	movl	44(%rsp), %ecx          # 4-byte Reload
	movl	%ecx, %r8d
	movq	%rax, %rcx
	movq	48(%rsp), %rdx          # 8-byte Reload
	movq	56(%rsp), %r9           # 8-byte Reload
	callq	_vfscanf_s_l
	movl	%eax, 76(%rsp)
	movl	76(%rsp), %eax
	addq	$88, %rsp
	retq
	.seh_handlerdata
	.section	.text,"xr",discard,scanf_s
	.seh_endproc
                                        # -- End function
	.def	 _vfscanf_s_l;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,_vfscanf_s_l
	.globl	_vfscanf_s_l            # -- Begin function _vfscanf_s_l
	.p2align	4, 0x90
_vfscanf_s_l:                           # @_vfscanf_s_l
.seh_proc _vfscanf_s_l
# %bb.0:
	subq	$104, %rsp
	.seh_stackalloc 104
	.seh_endprologue
	movq	%r9, 96(%rsp)
	movq	%r8, 88(%rsp)
	movq	%rdx, 80(%rsp)
	movq	%rcx, 72(%rsp)
	movq	96(%rsp), %rcx
	movq	88(%rsp), %r9
	movq	80(%rsp), %r8
	movq	72(%rsp), %rdx
	movq	%rcx, 64(%rsp)          # 8-byte Spill
	movq	%r9, 56(%rsp)           # 8-byte Spill
	movq	%r8, 48(%rsp)           # 8-byte Spill
	movq	%rdx, 40(%rsp)          # 8-byte Spill
	callq	__local_stdio_scanf_options
	movq	(%rax), %rax
	orq	$1, %rax
	movq	%rax, %rcx
	movq	40(%rsp), %rdx          # 8-byte Reload
	movq	48(%rsp), %r8           # 8-byte Reload
	movq	56(%rsp), %r9           # 8-byte Reload
	movq	64(%rsp), %rax          # 8-byte Reload
	movq	%rax, 32(%rsp)
	callq	__stdio_common_vfscanf
	nop
	addq	$104, %rsp
	retq
	.seh_handlerdata
	.section	.text,"xr",discard,_vfscanf_s_l
	.seh_endproc
                                        # -- End function
	.def	 __local_stdio_scanf_options;
	.scl	2;
	.type	32;
	.endef
	.section	.text,"xr",discard,__local_stdio_scanf_options
	.globl	__local_stdio_scanf_options # -- Begin function __local_stdio_scanf_options
	.p2align	4, 0x90
__local_stdio_scanf_options:            # @__local_stdio_scanf_options
# %bb.0:
	leaq	"?_OptionsStorage@?1??__local_stdio_scanf_options@@9@4_KA"(%rip), %rax
	retq
                                        # -- End function
	.section	.rdata,"dr",discard,"??_C@_02DPKJAMEF@?$CFd?$AA@"
	.globl	"??_C@_02DPKJAMEF@?$CFd?$AA@" # @"??_C@_02DPKJAMEF@?$CFd?$AA@"
"??_C@_02DPKJAMEF@?$CFd?$AA@":
	.asciz	"%d"

	.section	.bss,"bw",discard,"?_OptionsStorage@?1??__local_stdio_scanf_options@@9@4_KA"
	.globl	"?_OptionsStorage@?1??__local_stdio_scanf_options@@9@4_KA" # @"?_OptionsStorage@?1??__local_stdio_scanf_options@@9@4_KA"
	.p2align	3
"?_OptionsStorage@?1??__local_stdio_scanf_options@@9@4_KA":
	.quad	0                       # 0x0

	.section	.drectve,"yn"
	.ascii	" /FAILIFMISMATCH:\"_CRT_STDIO_ISO_WIDE_SPECIFIERS=0\""

	.addrsig
	.addrsig_sym scanf_s
	.addrsig_sym _vfscanf_s_l
	.addrsig_sym __acrt_iob_func
	.addrsig_sym __stdio_common_vfscanf
	.addrsig_sym __local_stdio_scanf_options
	.addrsig_sym "?_OptionsStorage@?1??__local_stdio_scanf_options@@9@4_KA"
