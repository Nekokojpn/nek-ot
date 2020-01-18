	.text
	.def	 @feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"sleep.cpp"
	.def	 "?sleep@@YAXH@Z";
	.scl	2;
	.type	32;
	.endef
	.globl	"?sleep@@YAXH@Z"        # -- Begin function ?sleep@@YAXH@Z
	.p2align	4, 0x90
"?sleep@@YAXH@Z":                       # @"?sleep@@YAXH@Z"
.seh_proc "?sleep@@YAXH@Z"
# %bb.0:
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	movl	%ecx, 36(%rsp)
	imull	$1000, 36(%rsp), %ecx   # imm = 0x3E8
	callq	*__imp_Sleep(%rip)
	nop
	addq	$40, %rsp
	retq
	.seh_handlerdata
	.text
	.seh_endproc
                                        # -- End function
	.section	.drectve,"yn"
	.ascii	" /DEFAULTLIB:uuid.lib"
	.ascii	" /DEFAULTLIB:uuid.lib"

	.addrsig
