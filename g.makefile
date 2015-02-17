# SPITBOL makefile using tcc
HOST=unix_64
#nasm?=tools/nasm/bin/nasm.$(HOST)
AS=as

ws?=64

debug?=0
DEBUG=$(debug)

EXECUTABLE=spitbol

os?=unix

it?=0
IT=$(it)
ifneq ($(IT),0)
ITOPT=:it
ITDEF=-Di_trace
endif

xt?=0
XT=$(xt)
ifneq ($(XT),0)
XTOPT=:xt
endif

OS=$(os)
WS=$(ws)
TARGET=$(OS)_$(WS)

# basebol determines which spitbol to use to compile
spitbol?=./bin/spitbol.$(HOST)
BASEBOL=$(spitbol)


cc?=gcc
CC=$(cc)

ifeq	($(DEBUG),1)
GFLAG=-g
endif


ARCH=-D$(TARGET)  -m$(WS)


ifeq ($(CC),tcc)
CC=tools/tcc/bin/tcc
#LIBS = -L$(MUSL)/lib  -Ltcc/lib/tcc/libtcc1.a $(MUSL)/lib/libc.a 
CCOPTS= $(ARCH) $(ITDEF) -I tools/tcc/include $(GFLAG) $(ITDEF)
LDOPTS = -Ltools/tcc/lib -Ltools/musl/lib $(GFLAG) 
LMOPT=-lm
else
CCOPTS= $(ARCH) $(ITDEF) $(GFLAG) 
LDOPTS= -lm $(GFLAG) $(ARCH)
LMOPT=-lm
endif

ifeq ($(OS),unix)
ELF=elf$(WS)
else
ELF=macho$(WS)
endif


# SPITBOL Version:
MIN=   s

# Minimal source directory.
MINPATH=./

OSINT=./osint

vpath %.c $(OSINT)

# Assembler info -- Intel 32-bit syntax
ifeq	($(DEBUG),0)
#ASOPTS = -Wa,m64
ASOPTS = -c --$(WS)
else
ASOPTS = -c --$(WS) -g
endif

# Tools for processing Minimal source file.
LEX=	lex.spt
ASM=    asm.spt
ERR=    err.spt

# Implicit rule for building objects from C files.
./%.o: %.c
#.c.o:
	$(CC)  $(CCOPTS) -c  -o$@ $(OSINT)/$*.c

# Implicit rule for building objects from assembly language files.
.s.o:
	$(AS) $(ASOPTS) -o$@ $*.s

# C Headers common to all versions and all source files of SPITBOL:
CHDRS =	$(OSINT)/osint.h $(OSINT)/port.h $(OSINT)/sproto.h $(OSINT)/spitio.h $(OSINT)/spitblks.h $(OSINT)/globals.h 

# C Headers unique to this version of SPITBOL:
UHDRS=	$(OSINT)/systype.h $(OSINT)/extern32.h $(OSINT)/blocks32.h $(OSINT)/system.h

# Headers common to all C files.
HDRS=	$(CHDRS) $(UHDRS)

# Headers for Minimal source translation:
VHDRS=	r.s.h s.h

OBJS=sysax.o sysbs.o sysbx.o syscm.o sysdc.o sysdt.o sysea.o \
	sysef.o sysej.o sysem.o sysen.o sysep.o sysex.o sysfc.o \
	sysgc.o syshs.o sysid.o sysif.o sysil.o sysin.o sysio.o \
	sysld.o sysmm.o sysmx.o sysou.o syspl.o syspp.o sysrw.o \
	sysst.o sysstdio.o systm.o systty.o sysul.o sysxi.o  \
	break.o checkfpu.o compress.o cpys2sc.o \
	doset.o dosys.o fakexit.o float.o flush.o gethost.o getshell.o \
	int.o lenfnm.o math.o optfile.o osclose.o \
	osopen.o ospipe.o osread.o oswait.o oswrite.o prompt.o rdenv.o \
	st2d.o stubs.o swcinp.o swcoup.o syslinux.o testty.o\
	trypath.o wrtaout.o it.o getargs.o main.o m.o err.o s.o



# link spitbol with static linking
spitbol: $(OBJS)
#	$(CC) $(LDOPTS) -static -lm $(OBJS) -o$(EXECUTABLE) 

#	$(CC) -static  $(LDOPTS)  $(OBJS) $(LMOPT) -o$(EXECUTABLE) 
	$(CC) $(LDOPTS)  $(OBJS) $(LMOPT) -o$(EXECUTABLE) 

# link spitbol with dynamic linking
spitbol-dynamic: $(OBJS)
	$(CC) $(LDOPTS) $(OBJS) $(LMOPT)  -ospitbol 

# bootbol is for bootstrapping just link with what's at hand
#bootbol: $(OBJS)
# no dependencies so can link for osx bootstrap
bootbol: 
	$(CC) $(LDOPTS)  $(OBJS) $(LMOPT) -obootbol

# Assembly language dependencies:
err.o: err.s

s.o: s.s

# SPITBOL Minimal source
s.go:	s.lex go.spt
	$(BASEBOL) -u i32 go.spt

r.s.s:	s.lex $(VHDRS) $(ASM) 
	$(BASEBOL) -u $(TARGET)$(ITOPT)$(XTOPT) $(ASM)

s.lex: $(MINPATH)$(MIN).min $(MIN).cnd $(LEX)
	 $(BASEBOL) -u $(TARGET) $(LEX)

s.err: s.s

err.s: $(MIN).cnd $(ERR) s.s
	   $(BASEBOL) -u $(TARGET) -1=s.err -2=err.s $(ERR)


m.h:	r.m.h
	$(BASEBOL) -u $(TARGET) r.spt < r.m.h > m.h
s.h:	r.s.h $(ASM)
	$(BASEBOL) -u $(TARGET) r.spt < r.s.h > s.h
m.s:	r.m.s r.m.h m.h $(ASM)
	$(BASEBOL) -u $(TARGET) r.spt < r.m.s > m.s
s.s:	r.s.s s.h m.h
	$(BASEBOL) -u $(TARGET) r.spt < r.s.s > s.s

# C language header dependencies:
main.o: $(OSINT)/save.h
sysgc.o: $(OSINT)/save.h
sysxi.o: $(OSINT)/save.h
dlfcn.o: dlfcn.h

# install binaries from ./bin as the system spitbol compilers
install:
	sudo cp ./bin/spitbol /usr/local/bin
clean:
	rm -f $(OBJS) *.o *.lst *.map *.err s.lex s.tmp r.s.s s.s err.s s.s s.h  m.h m.s ./spitbol

z:
	nm -n s.o >s.nm
	spitbol map-$(WS).spt <s.nm >s.dic
	spitbol z.spt <ad >ae

sclean:
# clean up after sanity-check
	make clean
	rm tbol*