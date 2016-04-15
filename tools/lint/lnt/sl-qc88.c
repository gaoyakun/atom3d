/*
 *      Standard Library Definitions -- QC88 Version    04/30/86
 *
 *  This file contains definitions of standard library functions
 *  for use with PC-Lint.  Use this file for input to PC-Lint.
 *  Do not compile this file and certainly never link it
 *  with a C program as it will null out the behavior of all
 *  C functions defined.
 */

#include "stdio.h"

typedef int BOOL;       /* TRUE/FALSE */
typedef char *UNIV;     /* universal pointer */
typedef char *STRING;   /* string */

#define VOID            /* function returns nothing */
typedef FILE *STREAM;   /* a universal file designator */

/*lint -save  save the current error suppression state */

/*lint -e715  Don't report on unused parameters. */
/*lint -e530  Don't report on uninitialized variables */
/*lint -e533  Don't insist on a return statement */

/*lint -library  library flag -- Don't get upset if these externals
	      are not referenced */

s_y_s_t_e_m()  /* This fake function defines arguments for main */
    {
    int argc;
    STRING *argv;
    main( argc, argv );
    }


VOID    abort() {}
int     atoi(s) STRING s; {}
UNIV    calloc( n, m ) unsigned n,m; {}
int     close(fd) int fd; {}
int     creat( fname, mode ) STRING fname; int mode; {}
VOID    _exit(n) int n; {}
VOID    exit(n) int n; {}
VOID    fclose(f) STREAM f; {}
BOOL    feof(f) STREAM f; {}
BOOL    ferror(f) STREAM f; {}
STRING  fgets( buf, len, f ) STRING buf; int len; STREAM f; {}
STREAM  fopen( name, mode ) STRING name, mode; {}
VOID    fputs( s, f) STRING s; STREAM f; {}
int     fread( p, sz, n, f ) UNIV p; int sz, n; STREAM f; {}
int     free( p ) UNIV p; {}
int     fwrite( p, sz, n, f) UNIV p; int sz, n; STREAM f; {}
int     getc( f ) STREAM f; {}
int     getchar() {}
STRING  gets( buf) UNIV buf; {}
STRING  index( s, c ) STRING s; char c; {}
BOOL    isalnum( c ) int c; {}
BOOL    isalpha( c ) int c; {}
BOOL    isdigit( c ) int c; {}
BOOL    isgraph( c ) int c; {}
BOOL    islower( c ) int c; {}
BOOL    isprint( c ) int c; {}
BOOL    isspace( c ) int c; {}
BOOL    isupper( c ) int c; {}
BOOL    isxdigit( c ) int c; {}
STRING  itob( n, s, b ) int n, b; STRING s; {}
UNIV    malloc(n) unsigned n; {}
int     open( name, mode ) STRING name; int mode; {}
int     putc( c, f) int c; STREAM f; {}
int     putchar( c) int c; {}
VOID    puts( s ) STRING s; {}
int     read( fd, buf, count ) int fd, count; UNIV buf; {}
int     remove( nm ) STRING nm; {}
STRING  rindex( s, c ) STRING s; char c; {}
UNIV    sbrk( n ) unsigned n; {}
VOID    setbuf( f, buf ) STREAM f; char *buf; {}
STRING  strcat( s1, s2 ) STRING s1, s2; {}
STRING  strchr( s, c ) STRING s; char c; {}
int     strcmp( s1, s2 ) STRING s1, s2; {}
STRING  strcpy( s1, s2 ) STRING s1, s2; {}
int     strlen( s ) STRING s; {}
STRING  strncat( s1, s2, n ) STRING s1, s2; int n; {}
STRING  strncpy( s1, s2, n ) STRING s1, s2; int n; {}
int     strncmp( s1, s2, n ) STRING s1, s2; int n; {}
STRING  strrchr( s, c ) STRING s; char c; {}
int     tolower( c ) int c; {}
int     toupper( c ) int c; {}
int     ungetc( c, f ) int c; STREAM f; {}
int     unlink( nm ) STRING nm; {}
int     write( fd, buf, count) int fd, count; UNIV buf; {}


/*
 * Q/C library internal variables
 */

char *_free;
char _version[];
STREAM stdin, stdout, stderr;

/*
 * Q/C library unique functions
 */

VOID    cantopen( nm ) STRING nm; {}
int     getkey() {}
int     imax( a, b ) int a, b; {}
int     imin( a, b ) int a, b; {}
unsigned maxsbrk() {}
unsigned moat(size) unsigned size; {}
int     seek( fd, off, base ) int fd, base; unsigned off; {}
VOID    setbsize( f, size ) STREAM f; int size; {}
STRING  strmov( s1, s2 ) STRING s1, s2; {}
unsigned tell( fd ) int fd; {}

/*lint -restore Restore error messages to "-save"d state */
/* end SL-QC88.C */
