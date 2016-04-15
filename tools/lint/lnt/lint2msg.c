/*
   EXAMPLE SOURCE CODE FOR GREP FILTER

   Grep2Msg.C
   Copyright (c) 1990, 1991 Borland International, Inc.
   All rights reserved.

   Grep2Msg - Message filter from Turbo Grep to Turbo C++ IDE message window

   This filter accepts input through the standard input stream, converts
   it and outputs it to the standard output stream.  The streams are linked
   through pipes, such that the input stream is the output from GREP, and
   the output stream is connected to the message window of the Turbo C++ IDE.
   This filter is invoked through the Turbo C++ IDE transfer mechanism as

            grep <commands> | grep2msg | TC IDE

    Compile using Turbo C++ in the LARGE memory model

    tcc -ml grep2msg
*/

#include <dir.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <alloc.h>
#include <io.h>
#include <dos.h>
#include <stdio.h>
#include "filter.h"

#define TRUE  1
#define FALSE 0

#define STR_MAX  256
#define NAME_MAX 80

char     NewFileText[] = "--- Module:   ";
char     IncludeText[] = "Including file ";
unsigned BufSize,CurBufLen;
char     *InBuffer,
	 *OutBuffer,
	 *CurInPtr,
	 *CurOutPtr,
	 *LinePtr;
char     Line[133];
long int InOff;
char     EndMark;
int      NoLines;
char     oldfile[NAME_MAX] = "";
int      replace = 0;

/************************************************************************
Function  : NextChar
Parameters: None
Returns   : next character in input buffer or 0 for end of file

Input from the standard input stream is buffered in a global buffer InBuffer
which is allocated in function main.  NextChar function will return
the next character in the buffer, reading from the input stream when the
buffer becomes empty.
************************************************************************/
char NextChar(void)
{
   if (CurInPtr < InBuffer+CurBufLen)   /* if buffer is not empty */
   {
      return *(CurInPtr++);             /* return next information */
   }
   else
   {
      CurInPtr = InBuffer;              /* reset pointer to front of buffer */
      lseek(0,InOff,0);                 /* seek to the next section for read */
      InOff += BufSize;                 /* increment pointer to next block */
      if ((CurBufLen = (unsigned) read(0,InBuffer,BufSize)) !=0)
	 if (CurInPtr < InBuffer+CurBufLen)
	     return *(CurInPtr++);
      return 0;                         /* return 0 on end of file */
   }
}

/*************************************************************************
Function  : flushOut
Parameters: Size   The number of characters to be written out
Returns   : nothing

Strings to be sent to the message window are placed in a buffer called
OutBuffer.  A call to this function will write Size bytes to the
standard output stream and reset the output buffer pointer to the
beginning of the buffer.  Any additional information in the buffer is
thus lost.
**************************************************************************/
void flushOut(unsigned Size)
{
  if (Size != 0)                 /* don't flush an empty buffer */
  {
    CurOutPtr = OutBuffer;       /* reset pointer to beginning of buffer */
    lseek(1,0L,2);                /* seek output stream to end */
    write(1,OutBuffer,Size);     /* write out Size bytes */
  }
}

/**************************************************************************
Function  : Put
Parameters: S     pointer to a string of characters
	    Len   length of the string of characters
Returns   : Nothing.

Put places bytes into OutBuffer so they may be later flushed out into the
standard output stream using flushOut.
If global variable 'replace' is set, any ':' in the string is replaced by
    a '|' because otherwise the IDE gets confused.
*************************************************************************/
void Put(char *S,int Len)
{
  int i;

  for (i = 0; i < Len; i++)
  {
    if( replace )
	*CurOutPtr++ = (S[i] == ':') ? '|' : S[i]; /* place byte in buffer */
    else
	*CurOutPtr++  = S[i];
    if (CurOutPtr >= OutBuffer+BufSize)      /* if buffer overflows */
      flushOut(BufSize);                     /* flush to the stream */
  }
}

/* parseline() will use strtok() to break up a line
 */
void parseline( const char *line, unsigned int *lno, unsigned int *col,
		char *filename, char *msg )
   {
   char temp[STR_MAX], *tmpptr, *t2;
   int num = 0;

   strcpy( temp, line );
   tmpptr = temp;
   t2 = strtok( tmpptr, " " );
   num += strlen( t2 ) + 1;
   *lno = (unsigned int) atoi( t2 );
   t2 = strtok( (char *) NULL, " " );
   num += strlen( t2 ) + 1;
   *col = (unsigned int) atoi( t2 );
   strcpy( filename, strtok( (char *) NULL, " " ) );
   num += strlen( filename ) + 1;
   strcpy( msg, line + num );
   }

/* newfile_block() will write out the information needed to declare a new
   file
 */
void newfile_block( char *filename )
    {
    char temp;

    temp = (char) MsgNewFile;  /* new file block */
    Put( &temp, 1 );
    Put( filename, strlen( filename ) + 1 );
    strcpy( oldfile, filename );
    NoLines = TRUE;
    }

/* newmesg_block() will write out the information for a new message
 */
void newmesg_block( char *msg, int lno, int col )
    {
    char temp;

    temp = (char) MsgNewLine;  /* new line block */
    Put( &temp, 1 );
    Put( (char *) &lno, 2 );
    Put( (char *) &col, 2 );
    replace = 1;
    Put( msg, strlen( msg ) + 1 );
    replace = 0;
    }

/**************************************************************************
Function  : ProcessLine
Parameters: Line   a pointer to the character line to be analyzed
Returns   : Nothing.

Filters lines output from grep into a format usable in the Turbo C++
environment message window.  Lines are simply sent straight through
with format characters for the message window.
**************************************************************************/
void ProcessLine( char *Line )
{
  unsigned i;
  unsigned col;
  unsigned lno;
  char msg[255];
  char filename[80];

  if( !Line || Line[0] == 0 )
      return;
  /* check for formatted output line */
  if( (Line[0] >= '0') && (Line[0] <= '9') )
      {
      NoLines = FALSE;
      parseline( Line, &lno, &col, filename, msg );
      col++;
      if( strncmp( filename, oldfile, NAME_MAX ) != 0 )
	  {
	  newfile_block( filename );
	  NoLines = TRUE;
	  }
      newmesg_block( msg, lno, col );
      }
   else
      {
      /* check for new module */
      if( strncmp( Line, NewFileText, strlen(NewFileText) ) == 0 )
	  {
	  memmove( filename, &Line[strlen( NewFileText )],
		   strlen( Line ) - strlen( NewFileText ) + 1 );
	  for( i = strlen(filename) - 1; filename[i] == ' ';
	       filename[i--] = '\0')
	      ;
	  newfile_block( filename );
	  NoLines = TRUE;
	  newmesg_block( Line, 1, 1 );
	  }
      else
	  {
	  /* check for included file */
	  if( strncmp( Line, IncludeText, strlen( IncludeText ) ) == 0 )
	      {
	      memmove( msg, &Line[strlen( IncludeText )],
		       strlen( Line ) - strlen( IncludeText ) + 1 );
	      sscanf(msg, "%[^ (\n]", filename);
	      newfile_block( filename );
	      newmesg_block( msg, 1, 1 );
	      }
	  else
	      newmesg_block( msg, 1, 1 );
	  }
      }
  }

/************************************************************************
Function  : Main

Returns   : zero on successful execution
	       3 on an error condition

The main routine allocates memory for the input and output buffers.
Characters are then read from the input buffer building the line buffer
that will be sent to the filter processor.  Lines are read and filtered
until the end of input is reached.
************************************************************************/
int main(void)
{
   char c;
   char MType;
   unsigned long core;
   unsigned lnum = 0;

   (void) setmode(1,O_BINARY);        /* set standard out to binary mode */
   NoLines = FALSE;                   /* No lines have been read yet */
   core = farcoreleft();              /* get available memory */
   if (core > 64000U)                 /* limit buffers to total of 64000 */
      BufSize = 64000U;               /* bytes */
   else
      BufSize = (unsigned)core;
   if ((InBuffer = (char *)  malloc(BufSize)) == NULL) /* allocate buffer space */
      exit(3);                        /* abort if error occured */
   CurInPtr = InBuffer;               /* split buffer */
   BufSize = BufSize/2;               /* between input and output buffers */
   OutBuffer = InBuffer + BufSize;
   CurOutPtr = OutBuffer;
   LinePtr = Line;                    /* set line buffer pointer */
   CurBufLen = 0;                     /* and reset buffer size to zero */
   Put(PipeId,PipeIdLen);             /* Identify process to message window */
   MType = (char) MsgNewFile;         /* indicate new file */
   Put( &MType, 1 );
   Put( " ", 2 );                     /* kill default filename */
   MType = (char) MsgNewLine;
   while ((c = NextChar()) != 0)      /* read characters */
   {
      if ((c == 13) || (c == 10))     /* build line until new line is seen */
      {
	 lnum++;
	 *LinePtr = 0;
	 if( lnum > 1 )
	     ProcessLine(Line);           /* then filter the line */
	 LinePtr = Line;
      }
      /* characters are added to line only up to 132 characters */
      else if ((FP_OFF(LinePtr) - FP_OFF(&Line)) < 132)
      {
	 *LinePtr = c;
	 LinePtr++;
      }
   }
   *LinePtr = 0;
   ProcessLine( Line );                  /* filter last line */
   if( NoLines )
       newmesg_block( " ", 1, 1 );
   EndMark = (char) MsgEoFile;           /* indicate end of input to */
   Put(&EndMark,1);                      /* message window */
   flushOut((unsigned)(CurOutPtr-OutBuffer));  /* flush out remaining buffer */

   return  0;                          /* everything went ok */
}
