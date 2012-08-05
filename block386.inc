;-------------------------- block386.inc -----------------------------
;.nolist
;       Version 0.90    18-Dec-89
;                       Initial version
;
;       Version 1.00    28-Feb-90
;                       First release version.  Add additional blocks.
;
;       Version 1.10    10-Sep-92
;                       Add string buffer blocks.
;
;       Version 1.20    19-Jun-09
;                       Modified for Linux 386 (32-bit) SPITBOL.
;
;
;       Copyright 1989 Catspaw, Inc.   All Rights Reserved.
;
;       SPITBOL BLOCKS
;
;       All SPITBOL objects are encapsulated into blocks of memory.
;       The first word of each block identifies the block type in a
;       curious way.  Rather than containing a simple integer type
;       code, it contains the address of a subroutine that performs
;       some particular action on the block, such as placing it on
;       the stack.  Because each block type uses a different subroutine,
;       the subroutine addresses serve as a type identifier.
;
;       Each subroutine is proceeded in memory by a one-byte integer
;       type code (given below).  Thus to obtain a simple ordinal
;       type code for a block pointed to by an address in EBX, the following
;       code sequence will load the ordinal value into register AL:
;
;       ....                    Block pointer in EBX
;       MOV     EAX,[EBX]       Type word (subroutine pointer) to EAX
;       MOV     AL,[EAX-1]      Load integer type code to AL
;
;       To obtain the type code as a full 32-bit integer, replace the last
;       instruction with:
;
;       MOVZX   EAX,BYTE PTR [EAX-1]    Load integer type code to EAX
;
;       +---------------------+               +------------------ +
;       |      Type Word      |----+          | Integer type byte |
;       +---------------------+    |          +-------------------+--------+
;       |      Block Data     |    +--------->|   Program Code for this    |
;      ...                   ...              |   Block Type               |
;       |                     |               |                            |
;       +---------------------+               +----------------------------+
;
;
;       Given an integer type, the type word can be obtained by
;       looking it up in a table provided to external functions in ptyptab
;       (provided in the miscellaneous area).
;       For example, if the ptyptab pointer is loaded into ES:ECX, then given
;       a small integer type code in EAX, the type word is obtained with:
;
;       MOV     EAX,ES:[ECX+EAX*4]     Obtain type word
;
;
;       BLOCK CODES FOR ACCESSIBLE DATATYPES
;
;       These blocks may appear in an argument list if left untranslated
;       by the LOAD function definition.
;
BL_AR  equ      0               ;ARBLK  ARRAY
BL_CD  equ      BL_AR+1         ;CDBLK  CODE
BL_EX  equ      BL_CD+1         ;EXBLK  EXPRESSION
BL_IC  equ      BL_EX+1         ;ICBLK  INTEGER
BL_NM  equ      BL_IC+1         ;NMBLK  NAME
BL_P0  equ      BL_NM+1         ;P0BLK  PATTERN, 0 args
BL_P1  equ      BL_P0+1         ;P1BLK  PATTERN, 1 arg
BL_P2  equ      BL_P1+1         ;P2BLK  PATTERN, 2 args
BL_RC  equ      BL_P2+1         ;RCBLK  REAL
BL_SC  equ      BL_RC+1         ;SCBLK  STRING
BL_SE  equ      BL_SC+1         ;SEBLK  EXPRESSION
BL_TB  equ      BL_SE+1         ;TBBLK  TABLE
BL_VC  equ      BL_TB+1         ;VCBLK  VECTOR (array)
BL_XN  equ      BL_VC+1         ;XNBLK  EXTERNAL, non-relocatable contents
BL_XR  equ      BL_XN+1         ;XRBLK  EXTERNAL, relocatable contents
BL_BC  equ      BL_XR+1         ;BCBLK  BUFFER CONTROL
BL_PD  equ      BL_BC+1         ;PDBLK  PROGRAM DEFINED DATATYPE

BL__D  equ      BL_PD+1         ;NUMBER OF BLOCK CODES FOR DATA
;
;
;       OTHER BLOCK CODES
;
;       These blocks will never appear in an argument list, but are
;       listed here for completeness.
;
BL_TR  equ      BL_PD+1         ;TRBLK  TRACE
BL_BF  equ      BL_TR+1         ;BFBLK  STRING BUFFER
BL_CC  equ      BL_BF+1         ;CCBLK  CODE CONSTRUCTION
BL_CM  equ      BL_CC+1         ;CMBLK  COMPILER TREE NODE
BL_CT  equ      BL_CM+1         ;CTBLK  CHARACTER TABLE
BL_DF  equ      BL_CT+1         ;DFBLK  DATATYPE FUNCTION
BL_EF  equ      BL_DF+1         ;EFBLK  EXTERNAL FUNCTION
BL_EV  equ      BL_EF+1         ;EVBLK  EXPRESSION VARIABLE
BL_FF  equ      BL_EV+1         ;FFBLK  FIELD FUNCTION
BL_KV  equ      BL_FF+1         ;KVBLK  KEYWORD VARIABLE
BL_PF  equ      BL_KV+1         ;PFBLK  PROGRAM-DEFINED FUNCTIN
BL_TE  equ      BL_PF+1         ;TEBLK  TABLE ELEMENT
;
;
;
;       Structure of common SPITBOL blocks:
;         Integer, Real, String, and File blocks.
;
;       These structures can be applied to the result area to determine
;       where to store required return information.
;
;
;       Structure of ICBLK (integers)
;
        segment         .data
        struc   icblk
ictyp:  resd    1       ;type word
icval:  resd    1       ;integer value
        endstruc    
;
;
;       Structure of RCBLK (reals)
;
        struc   rcblk
rctyp:  resd    1       ;type word
rcval:  resd    1       ;real value
        endstruc   
;
;       Structure of SCBLK (strings)
;
        struc   scblk
sctyp:  resd    1       ;type word
sclen:  resd     1       ;string length
scstr:  resd    1       ;start of string
        endstruc  
;
;       Structure for returning a string via a pointer
;
        struc   fsblk
fstyp:  resd    1       ;type word
fslen:  resd    1       ;string length
fsptr:  resd    1       ;near pointer to string
        endstruc 
;
;       Structure for returning an external block via a pointer
;
        struc   fxblk
fxtyp:  resd    1       ;type word
fxlen:  resd    1       ;external data length
fxptr   resd    1       ;near pointer to external data
        endstruc 
;
;
;       FILE CONTROL BLOCK
;
;       The user may provide the word "FILE" for any argument in
;       the LOAD function prototype.  When a call is made to the
;       external function with an I/O associated variable in this argument
;       position, SPITBOL will provide a pointer to the file control
;       block instead of the value of the variable.
;
;       The file control block (FCB) points to an I/O block with
;       additional information.  In turn, the I/O block points to any
;       buffer used by the file.
;
;       This block is obtained for every file except those associated
;       with INPUT, OUTPUT, or TERMINAL.  Note that these FCB's are
;       unrelated to MS-DOS FCB's.  File control blocks do not have
;       their own type word, but appear as XRBLKs with the following structure:

;
        struc   fcblk
fcbtyp: resd    1       ;type word (XRBLK)
fcblen: resd    1       ;size of block, in bytes
fcbrsz: resd    1       ;SPITBOL record size and mode
                        ;positive if text mode, negative if binary
fcbiob: resd    1       ;pointer to IOBLK
fcbmod: resd    1       ;1 if text mode, 0 if binary mode
        endstruc
;
;       I/O BLOCK
;
;       An I/O block is pointed to by the fcbiob field of a file control block.
;
        struc   ioblk
iobtyp:         resd    1       ;type word (XRBLK)
ioblen:         resd    1       ;size of IOBLK in bytes
iobfnm:         resd    1       ;pointer to SCBLK holding filename
iobpid:         resd    1       ;process id for pipe (not used for DOS)
iobbfb:         resd    1       ;pointer to BFBBLK
iobfdn:         resd    1       ;O/S file descriptor number
iobflg1:        resd    1       ;flags 1 (see below)
iobflg2:        resd    1       ;flags 2 (see below)
iobeol1:        resd    1       ;end of line character 1
iobeol2:        resd    1       ;end of line character 2
iobshare:       resd    1       ;sharing mode
iobaction:      resd    1       ;file open actions
        endstruc

;       Bits in iobflg1 dword:
;
IO_INP equ      0x00000001       ;input file
IO_OUP equ      0x00000002       ;output file
IO_APP equ      0x00000004       ;append output to end of existing file
IO_OPN equ      0x00000008       ;file is open
IO_COT equ      0x00000010       ;console output to non-disk device
IO_CIN equ      0x00000020       ;console input from non-disk device
IO_SYS equ      0x00000040       ;system file descriptor used instead of name
IO_WRC equ      0x00000080       ;output without buffering
IO_EOT equ      0x00000100       ;ignore end-of-text (control-Z) character

;       Bits in iobflg2 dword:
;
IO_PIP equ      0x00000001       ;pipe (not used in MS-DOS)
IO_DED equ      0x00000002       ;dead pipe (not used in MS-DOS)
IO_ILL equ      0x00000004       ;illegal I/O association
IO_RAW equ      0x00000008       ;binary I/O
IO_LF  equ      0x00000010       ;ignore line feed if next character
IO_NOE equ      0x00000020       ;no echo input
IO_ENV equ      0x00000040       ;filearg1 maps to filename thru environment var
IO_DIR equ      0x00000080       ;buffer is dirty (needs to be written)
IO_BIN equ      0x00000100       ;binary I/O

;       Bits in iobshare dword:
IO_COMPATIBILITY  equ      0x00
IO_DENY_READWRITE equ      0x01
IO_DENY_WRITE     equ      0x02
IO_DENY_READ      equ      0x03
IO_DENY_NONE      equ      0x04
IO_DENY_MASK      equ      0x07  ;mask for above deny mode bits
IO_EXECUTABLE     equ      0x40  ;file to be marked executable
IO_PRIVATE        equ      0x80  ;file is private to current process

;       Bits in iobaction dword:
IO_FAIL_IF_EXISTS     equ  0x00
IO_OPEN_IF_EXISTS     equ  0x01
IO_REPLACE_IF_EXISTS  equ  0x02
IO_FAIL_IF_NOT_EXIST  equ  0x00
IO_CREATE_IF_NOT_EXIST equ  0x10
IO_EXIST_ACTION_MASK  equ  0x13  ;mask for above bits
IO_WRITE_THRU         equ  0x20  ;writes complete before return

;
;       I/O BUFFER BLOCK
;
;       An I/O buffer block (BFBBLK) is pointed to by an IOBLK.
;
        struc   bfbblk
bfbtyp:         resd    1       ;type word (XNBLK)
bfblen:         resd    1       ;size of BFBBLK, in bytes
bfbsiz:         resd    1       ;size of buffer in bytes
bfbfil:         resd    1       ;number of bytes currently in buffer
bfbnxt:         resd    1       ;offset of next buffer char to r/w
bfboff:         resd    1       ;file position of first byte in buf
bfbcur:         resd    1       ;physical file position
bfbbuf:         resd    1       ;start of buffer
        endstruc
;
;
;       Structure of EFBLK (external function).  A pointer to this block
;       is passed to the external function in the miscellaneous area
;       in pefblk.
;
        struc   efblk
fcode:  resd    1       ;type word
fargs:  resd    1       ;number of arguments
eflen:  resd    1       ;block length
efuse:  resd    1       ;usage count
efcod:  resd    1       ;pointer to XNBLK, see below
efvar:  resd    1       ;pointer to VRBLK with function name
efrsl:  resd    1       ;result type  (see below)
eftar:  resd    1       ;argument types, one dword per arg (see below)
        endstruc
;
;  efrsl and eftar[] dwords contain small integer type codes as follows:
;  0 - unconverted
;  1 - convert to string
;  2 - convert to integer
;  3 - convert to real
;  4 - produce as file control block associated with variable
;
;
;       Structure of XNBLK allocated for external function
;       A pointer to this structure is passed to the external function
;       in the miscellaneous area in pxnblk.
;

        struc   xnblk           ;keep as multiple of dwords
xntyp:  resd    1       ;type word
xnlen:  resd    1       ;length of this block
xnhand: resd    1       ;module handle
xnpfn:  resd    1       ;pointer to function
xn1st:  resd    1       ;non-zero = first-ever call
xnsave: resd    1       ;non-zero = first call after reload of save file
xncbp:  resd    1       ;call back address prior to saving or exiting
        endstruc
xndta   equ       (xnlen+4)       ;start of user data region in XNBLK
;

;
;  Other selected blocks of interest:
;
;
;       ARRAY BLOCK
;
;       An array block (ARBLK) represents an array value other than one
;       with one dimension whose lower bound is one (see VCBLK).
;
        struc   arblk
artyp:  resd    1       ;type word (ARBLK)
aridv:  resd    1       ;identifier value
arlen:  resd    1       ;length of ARBLK in bytes
arofs:  resd    1       ;offset in arblk to arpro field
arndm:  resd    1       ;number of dimensions
arlbd:  resd    1       ;low bound (first subscript)
ardim:  resd    1       ;dimension (first subscript)
; above two dwords are repeated for each additional dimension
arpro:  resd    1       ;array prototype string
arvls:  resd    1       ;start of values in row-wise order
        endstruc

;       BUFFER CONTROL BLOCK
;
;       A buffer control block (BCBLK) is created by the BUFFER
;       function, and serves as an indirect control header for the
;       buffer. It contains the number of characters currently
;       stored in the buffer.
;
        struc   bcblk
bctyp:  resd    1       ;type word (BCBLK)
bcidv:  resd    1       ;identifier value
bclen:  resd    1       ;number of chars in use in bfblk
bcbuf:  resd    1       ;pointer to bfblk
        endstruc

;       STRING BUFFER BLOCK
;
;       A string buffer block (BFBLK) contains the actual buffer
;       memory area. It specifies the largest string that can be
;       stored in the buffer.
;
        struc   bfblk
bftyp:  resd    1       ;type word (BFBLK)
bfalc:  resd    1       ;allocated size of buffer
bfchr:  resd    1       ;characters of string
        endstruc

;
;       CODE BLOCK
;
;       A code block (CDBLK) is present for every source statement.
;
        struc   cdblk
cdjmp:  resd    1               ;ptr to routine to execute statement
cdstm:  resd    1               ;statement number
cdsln:  resd    1               ;source file line number
cdlen:  resd    1               ;length of CDBLK in bytes
cdfal:  resd    1               ;failure exit
cdcod:  resd    1               ;start of executable pseudo-code
        endstruc

;
;       NAME BLOCK
;
;       A name block (NMBLK) is used whereever a name must be stored as
;       a value following use of the unary dot operator.
;
        struc   nmblk
nmtyp:  resd    1               ;type word (NMBLK)
nmbas:  resd    1               ;base pointer for variable
nmofs:  resd    1               ;offset for variable
        endstruc

;
;       TABLE BLOCK
;
;       A table block (TBBLK) is used to represent a table value.
;       It comprises a list of buckets, each of which may point to
;       a chain of TEBLKs.  TBBUK entries either point to the first
;       TEBLK on the chain or they point to the TBBLK itself to
;       indicate the end of the chain.  The number of buckets can
;       be deduced from tblen.
;
        struc   tbblk
tbtyp:  resd    1               ;type word (TBBLK)
tbidv:  resd    1               ;identifier value
tblen:  resd    1               ;length of TBBLK in bytes
tbinv:  resd    1               ;default initial lookup value
tbbuk:  resd    1               ;start of hash bucket pointers
        endstruc

;
;       TABLE ELEMENT BLOCK
;
;       A table element (TEBLK) is used to represent a single entry in
;       a table.
;
        struc   teblk
tetyp:  resd    1               ;type word (TEBLK)
tesub:  resd    1               ;subscript value
teval:  resd    1               ;table element value
tenxt:  resd    1               ;link to next TEBLK or TBBLK if end of chain
        endstruc

;       VECTOR BLOCK
;
;       A vector block (VCBLK) is used to represent an array value which has
;       one dimension whose lower bound is one. All other arrays are
;       represented by ARBLKs.  The number of elements can be deduced
;       from vclen.
;
        struc   vcblk
vctyp:  resd    1               ;type word (VCBLK)
vcidv:  resd    1               ;identifier value
vclen:  resd    1               ;length of vcblk in bytes
vcvls:  resd    1               ;start of vector values
        endstruc

;.list
;---------------------  end of block386.inc -----------------------