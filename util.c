/*
  manyex: a program to produce exams from a database of exercises

  History:

  The inspiration and some code comes for the program "much":
  Copyright (C) 2004  Mihail N. Kolountzakis

  This program:
  Copyright (C) 2006-2010  Walter Garcia-Fontes

  Read the file COPYRIGHT for the complete copyright.

  Various utility subroutines.
*/

#include	"vars.h"
#define         BSIZE 512

// Substring function used in yacc for various things
substr(char dest[], char src[], int offset, int len)
{
  int i;
  for(i = 0; i < len && src[offset + i] != '\0'; i++)
    dest[i] = src[i + offset];
  dest[i] = '\0';
}

// generate a random subset of k elements of the set {0, ..., N-1} and
// return the elements in the vector elem
void    random_set(int N, int k, int *elem)
{
  int     n=0, i, r;
  char    *s;
  double  x;

  s = (char*)malloc(N);
  for(i=0; i<N; i++) s[i]=0;

  while(n<k) {
    x = RandomUniform();
    // x = drand48();
    x *= N;
    r = (int)x;
    if(s[r]) continue;
    s[r] = 1;
    elem[n++] = r;
  }
  free(s);
}

/*
  Simple function to copy the auxiliary files from the database to the 
  current directory. TODO: improve error handling in this file
*/
copyfile(from, to)
     char *from, *to;
{
struct stat stbuf1, stbuf2;
char iobuf[BSIZE];
  int fold, fnew, n;
  register char *p1, *p2, *bp;
  int mode;
  if ((fold = open(from, 0)) < 0) {
    fprintf(stderr, "cp: cannot open %s\n", from);
    return(1);
  }
  fstat(fold, &stbuf1);
  mode = stbuf1.st_mode;
  /* is target a directory? */
  if (stat(to, &stbuf2) >=0 &&
      (stbuf2.st_mode&S_IFMT) == S_IFDIR) {
    p1 = from;
    p2 = to;
    bp = iobuf;
    while(*bp++ = *p2++)
      ;
    bp[-1] = '/';
    p2 = bp;
    while(*bp = *p1++)
      if (*bp++ == '/')
	bp = p2;
    to = iobuf;
  }
  if (stat(to, &stbuf2) >= 0) {
    if (stbuf1.st_dev == stbuf2.st_dev &&
	stbuf1.st_ino == stbuf2.st_ino) {
      fprintf(stderr, "cp: cannot copy file to itself.\n");
      return(1);
    }
  }
  if ((fnew = creat(to, mode)) < 0) {
    fprintf(stderr, "cp: cannot create %s\n", to);
    close(fold);
    return(1);
  }
  while(n = read(fold,  iobuf,  BSIZE)) {
    if (n < 0) {
      fprintf(stderr, "cp: read error\n");
      close(fold);
      close(fnew);
      return(1);
    } else
      if (write(fnew, iobuf, n) != n) {
	fprintf(stderr, "cp: write error.\n");
	close(fold);
	close(fnew);
	return(1);
      }
  }
  close(fold);
  close(fnew);
  return(0);
}




// Simple function to convert integer to string
char *itoa(int a,char *buffer, int buff_size) 
{ 
  char *temp; 
  int b; 
  temp = buffer; 

  b = a; 
  if (buffer == NULL) 
    return NULL; 

  while (a > 0) 
    { 
      if (buff_size < 1) 
	return NULL; 

      b = a%10; 
      *temp = b + '0' ; 
      temp++; 
      a = a/10; 
      buff_size--; 
    } 
  *temp = '\0'; 
  /*Note : You have to Add a String reverse Function on this buffer here*/ 
  return buffer; 
} 

