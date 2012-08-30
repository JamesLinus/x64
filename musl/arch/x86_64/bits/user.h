#undef __WORDSIZE
#define __WORDSIZE 64

struct user_fpregs_struct
{
	uint16_t cwd, swd, ftw, fop;
	uint64_t rip, rdp;
	uint32_t mxcsr, mxcs_mask;
	uint32_t st_space[32], xmm_space[64], padding[24];
};

struct user_regs_struct
{
	unsigned long r15, r14, r13, r12, rbp, rbx, r11, r10, r9, r8;
	unsigned long rax, rcx, rdx, rsi, rdi, orig_rax, rip;
	unsigned long cs, eflags, rsp, ss, fs_base, gs_base, ds, es, fs, gs;
};

struct user
{
	struct user_regs_struct		regs;
	int				u_fpvalid;
	struct user_fpregs_struct	i387;
	unsigned long			u_tsize;
	unsigned long			u_dsize;
	unsigned long			u_ssize;
	unsigned long			start_code;
	unsigned long			start_stack;
	long				signal;
	int				reserved;
	struct user_regs_struct		*u_ar0;
	struct user_fpregs_struct	*u_fpstate;
	unsigned long			magic;
	char				u_comm[32];
	unsigned long			u_debugreg[8];
};

#define PAGE_MASK		(~(PAGE_SIZE-1))
#define NBPG			PAGE_SIZE
#define UPAGES			1
#define HOST_TEXT_START_ADDR	(u.start_code)
#define HOST_STACK_END_ADDR	(u.start_stack + u.u_ssize * NBPG)