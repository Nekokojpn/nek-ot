	.text
	.def	 @feat.00;
	.scl	3;
	.type	0;
	.endef
	.globl	@feat.00
.set @feat.00, 0
	.file	"rand.cpp"
	.def	 "?random@@YAHXZ";
	.scl	2;
	.type	32;
	.endef
	.globl	"?random@@YAHXZ"        # -- Begin function ?random@@YAHXZ
	.p2align	4, 0x90
"?random@@YAHXZ":                       # @"?random@@YAHXZ"
.seh_proc "?random@@YAHXZ"
# %bb.0:
	subq	$40, %rsp
	.seh_stackalloc 40
	.seh_endprologue
	callq	rand
	nop
	addq	$40, %rsp
	retq
	.seh_handlerdata
	.text
	.seh_endproc
                                        # -- End function

	.addrsig
	.addrsig_sym rand
