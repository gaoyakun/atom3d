/*lint -cdesm

    Standard Library Definitions -- Desmet Version

    This file contains declarations of standard library functions
    for use with PC-Lint.  When provided to PC-Lint along with
    other modules, the functions declared here are considered
    defined with the properties given.  Of course, later redefinitions
    may occur.

    This file may not be complete.  Your compiler may contain
    many other functions not declared here.
    You may modify or augment this file so that it more
    closely conforms to the library provided with your compiler.

    The include file "stdio.h" is provided with your compiler.

 */

/*lint +fcu         character is unsigned */
/*lint +fsu         strings are made of unsigned chars */
/*lint -t4          tab = 4, for users of DeSmet SEE Editor*/
/*lint -e612        allows static struct name {...}; which has a
                    peculiar meaning in DeSmet C */

/*lint -library  library flag -- Don't get upset if these externals
              are not referenced or defined */


#include "stdio.h"

typedef void *UNIV;     /* universal pointer */
typedef char *STRING;   /* string */
typedef const char *CSTRING;   /* constant string */

#define VOID void       /* compiler-independent void */
typedef FILE *STREAM;   /* a universal file designator */


/*lint +fvr
  The following functions exhibit variable return modes.
  That is, they may equally-usefully be called for a value
  as called just for their effects.
 */


int     close();
int     creat();
int     fclose();
int     fprintf();
int     fputc();
int     fputs();
int     fscanf();
long    fseek();
int     fwrite();
long    lseek();
UNIV    memcpy();
UNIV    memmove();
UNIV    memset();
int     printf();
int     puts();
int     scanf();
int     sprintf();
int     sscanf();
STRING  strcat();
STRING  strcpy();
STRING  strncat();
STRING  strncpy();
int     unlink();
int     write();


/*lint -fvr  End of routines that exhibit varying return mode */


VOID    main( int, char**);  /* since main is not otherwise referenced */

/*  At this point we wish to include prototypes for all functions
    you are likely to use.  The easiest way to do this is to use
    the prototypes that come with your compiler.

    Some of the less-frequently-used include files have been commented
    out to save processing time.  Also, for some compilers, there
    can be an overlap in that some functions are included in more
    than one header file.  Please include the omitted include files
    if you need them.  Failure to include an appropriate header will
    result in a Warning 526, ... "not defined" for any called function
    whose prototype is contained in the header.
 */


/* #include "assert.h" */
#include "ctype.h"
/* #include "dos.h" */
/* #include "float.h" */
/* #include "limits.h" */
/* #include "math.h" */
/* #include "setjmp.h" */
/* #include "stdarg.h" */
/* #include "signal.h" */
#include "stdlib.h"
#include "string.h"


/* The following functions are DeSmet specific (some are in pcio.a) */
/* Furnished by the staff of "The C Gazette" */

VOID    chain(STRING, STRING);
char    ci();
VOID    co(char);
char    csts();
VOID    dates(STRING);
VOID    _doint(int);
int     exec(STRING, STRING);
int     _gets(STRING, int);
char    _inb(unsigned);
unsigned    _inw (unsigned);
STRING  index(STRING, char);
VOID    _lmove(unsigned, UNIV, unsigned, UNIV, unsigned);
UNIV    _memory();
int     moverlay(int);
VOID    _move(unsigned, UNIV, UNIV);
char    _os(char, unsigned);
VOID    _outb(char, unsigned);
VOID    _outw(unsigned, unsigned);
int     overlay(int);
int     overlay_close();
int     overlay_init(STRING);
char    _peek(UNIV, unsigned);
char    _poke(char, UNIV, unsigned);
STRING  rindex(STRING, char);
VOID    scr_aputs (STRING, char);
char    scr_ci();
VOID    scr_co(char);
char    scr_csts();
VOID    scr_clr();
VOID    scr_clrl();
VOID    scr_cls();
VOID    scr_cursoff();
VOID    scr_curson();
VOID    scr_rowcol();
VOID    scr_scdn();
VOID    scr_scrdn(int, int, int, int, int);
VOID    scr_scup();
VOID    scr_scrup(int, int, int, int, int);
VOID    scr_setmode(char);
VOID    scr_setup();
char    scr_sinp();
VOID    _setmem(UNIV, unsigned, char);
VOID    _setsp(UNIV);
unsigned    _showcs();
unsigned    _showds();
unsigned    _showsp();
VOID    times(STRING);

/* The following functions were added in DeSmet release 3.0
   and do not appear to be mentioned in the .h files */
int     chdir(STRING);
int     chmod(STRING, int);
int     dup(int);
int     dup2(int);
STRING  getdir (char, STRING);
int     isatty(int);
int     locking(int, int, int);
int     mkdir (STRING);
int     rename(STRING, STRING);
STRING  strpbrk(STRING, STRING);




/*lint -restore Restore error messages to original state.
       This option is only necessary if flags of the form
       -e... appear in this file. */

