/*
  manyex: a program to produce exams from a database of exercises

  History:

  The inspiration and some code comes for the program "much":
  Copyright (C) 2004  Mihail N. Kolountzakis

  This program:
  Copyright (C) 2006-2010  Walter Garcia-Fontes

  Read the file COPYRIGHT for the complete copyright.

  Include, macros and general definitions.
*/


#include        <stdio.h>
#include        <ctype.h>
#include        <string.h>
#include        <locale.h>
#include        <fnmatch.h>
#include        <dirent.h>
#include        <stdlib.h>
#include        <math.h>            
#include        <stdarg.h>
#include        <getopt.h>
#include        <errno.h>
#include        <sys/types.h> // needed for copyfile subroutine
#include        <sys/stat.h> // needed for copyfile subroutine

#include	"randomlib.h"


#ifdef  MAIN_PROGRAM
#define EXTERN /**/
#else
#define EXTERN extern
#endif

#define	MAX_STR_LENGTH	        200  // max string length of lines
#define MAX_STR_LABEL_TF        50 // max string for truefalse labels
#define	MAX_FILENAME_LENGTH	200    // max allowed name for files
#define MAX_FILES_BROWSE        1000  // max files to browse for exercises
#define MAX_AUXILIARY_FILES     10    // max auxiliary files
#define MAX_EXERCISES_EXAM      20    // max exercices in an exam
#define MAX_BLOCKS_EXERCISE     20    // max blocks in an exercise
#define MAX_LINESTATEMENTS_EXERCISE 100 // max lines in exercise statements
#define MAX_LINESTATEMENTS_QUESTION 50 // max lines in question statements
#define MAX_LINESTATEMENTS_ANSWER 50 // max lines in answer statements
#define MAX_QUESTIONS_BLOCK     20    // max questions in a block
#define MAX_OPTIONS_QUESTION    15    // max choices in multiplechoice
#define MAX_INIFIELDS_FORM      15    // max initial fields for the web form


// The following macro prepends the basedir path to a filename
static	char	__REL__[1000];
#define	REL(s)	(sprintf(__REL__,"%s/%s", basedir, s),__REL__)


// definitions for bison
extern  int     exerlineno;
extern	FILE	*exerin;

extern  int     desclineno;
extern	FILE	*descin;

// file handels 
EXTERN FILE    *llog,\
  *out,              \
  *corr,             \
  *ans,
  *outw,
  *answ; // (llog=logfile, out=latex output, corr: anwer key, 
         // ans= latex answers, outw=web output, answ=web answers

// to control if we are creating LaTeX or web output
EXTERN int modenormal;
EXTERN int modeweb;

// a string for multiple dirs
EXTERN int checkdirw
#ifdef MAIN_PROGRAM
=0 
#endif
;

// some strings to hold parsed statements and other tasks
EXTERN	char title[MAX_STR_LENGTH];
EXTERN	char temp[MAX_STR_LENGTH];
EXTERN  char temp2[MAX_STR_LENGTH];

// strings to hold a lot of text for general use
EXTERN	char	line[MAX_OPTIONS_QUESTION][10*MAX_STR_LENGTH];

// structure to hold questions of a block in an exercise

// Base directory
EXTERN	char basedir[MAX_STR_LENGTH]
#ifdef MAIN_PROGRAM
="." // default
#endif
;

// Documentclass options
EXTERN	char documentclassoptions[MAX_STR_LENGTH]
#ifdef MAIN_PROGRAM
="12pt,a4paper" //default
#endif
;

// Master latex file (a file with extension ".sty" has to be created)
EXTERN	char mastertex[MAX_FILENAME_LENGTH]
#ifdef MAIN_PROGRAM
="master" // default
#endif
;


// Packages included in latex file
EXTERN	char usepackage[MAX_STR_LENGTH]
#ifdef MAIN_PROGRAM
=" " // default
#endif
;

// Number of master files included
// Fields for formweb
EXTERN  int masterfilenum
#ifdef MAIN_PROGRAM
=0 // initial value
#endif
;

// Exercise prefix for database of exercises
EXTERN char exerprefix[MAX_STR_LENGTH]
#ifdef MAIN_PROGRAM
="exer" // default
#endif
;

// Title for the html page
EXTERN	char pagetitle[MAX_STR_LENGTH]
#ifdef MAIN_PROGRAM
="Exam" // default
#endif
;

// Action for formweb
EXTERN	char actionfield[MAX_STR_LENGTH];

EXTERN  int solution
#ifdef MAIN_PROGRAM
=0 // flag to see if solutions are to be included in the form
#endif
;

// Fields for formweb
EXTERN  int inifieldnum
#ifdef MAIN_PROGRAM
=0 // initial value
#endif
;

EXTERN char namefield[MAX_INIFIELDS_FORM][MAX_FILENAME_LENGTH];
EXTERN char labelfield[MAX_INIFIELDS_FORM][MAX_FILENAME_LENGTH];
EXTERN char typefield[MAX_INIFIELDS_FORM][MAX_FILENAME_LENGTH];
EXTERN char sizefield[MAX_INIFIELDS_FORM][MAX_FILENAME_LENGTH];
EXTERN char textfield[MAX_INIFIELDS_FORM][MAX_STR_LENGTH];
EXTERN char textrequired[MAX_INIFIELDS_FORM][MAX_STR_LENGTH];
EXTERN char fieldname[MAX_INIFIELDS_FORM][MAX_STR_LENGTH];
EXTERN char printfield[MAX_INIFIELDS_FORM][MAX_STR_LENGTH];
EXTERN char printtextreq[MAX_INIFIELDS_FORM][MAX_STR_LENGTH];
EXTERN int  fieldrequired[MAX_INIFIELDS_FORM];
EXTERN int  checkrequired
#ifdef MAIN_PROGRAM
=0 // initial value
#endif
;
EXTERN char uncheck[MAX_STR_LENGTH];
EXTERN char textsubmit[MAX_STR_LENGTH];

// File prefix to output exams (the program will append ".tex")
EXTERN	char examtex[MAX_FILENAME_LENGTH]
#ifdef MAIN_PROGRAM
="exam" // default
#endif
;

// File prefix to output exams (the program will append ".tex")
EXTERN	char answertex[MAX_FILENAME_LENGTH]
#ifdef MAIN_PROGRAM
="answer" // default
#endif
;

// random seed
EXTERN	int	seed
#ifdef	MAIN_PROGRAM
=12345 // default
#endif
;

// various counters
EXTERN  int  i
#ifdef	MAIN_PROGRAM
=0 
#endif
;
EXTERN  int  j
#ifdef	MAIN_PROGRAM
=0 
#endif
;
EXTERN  int  k
#ifdef	MAIN_PROGRAM
=0 
#endif
;
EXTERN int chran // to be used to cycle over exerp to get random files
#ifdef	MAIN_PROGRAM
=0
#endif
;
EXTERN int numrequired // to be used to count the number of required fields
#ifdef	MAIN_PROGRAM
=0
#endif
;

// various options set in the exercise files
EXTERN int type_block; // will be 1="M", 2="S" or 3="T" 
EXTERN int truefalse; // 0=true, 1=false

EXTERN char label_tf[ ][MAX_STR_LABEL_TF]
#ifdef MAIN_PROGRAM
={"Choose","True","False"}
#endif
; // to hold labels of truefalse

EXTERN	int	numexam;  // number of exams to be created
EXTERN  int     exam
#ifdef	MAIN_PROGRAM
=0 // exam number being created (serial)
#endif
;
EXTERN  int  exer
#ifdef	MAIN_PROGRAM
=0 // count for total exercises
#endif
;
EXTERN  int  realexer
#ifdef	MAIN_PROGRAM
=0 // count for real total exercises (using EXERCISE ENDEXERCISE);
#endif
;
EXTERN  int blocknum
#ifdef	MAIN_PROGRAM
=0 // block number within exercise
#endif
;
EXTERN  int     numexer;  // number of exercises in exam
EXTERN  int     singleexer
#ifdef	MAIN_PROGRAM
=0 // to use with EXERCISE and ENDEXERCISE
#endif
;
EXTERN  int     numtype;  // number of types
EXTERN  int     numdir[MAX_EXERCISES_EXAM]; // number of dirs for a given type
EXTERN  int     iniserial
#ifdef	MAIN_PROGRAM
=1 // initial serial number of papers 
#endif
;

EXTERN  int     countques; // number of questions
EXTERN  int     utilcount // a general int to pass info between subroutines
#ifdef	MAIN_PROGRAM
=0 // block number within exercise
#endif
;

// array to hold names of dirs with questions (max dirs to browse: 20)
EXTERN char type[MAX_EXERCISES_EXAM][20][MAX_FILENAME_LENGTH];
EXTERN int numqtype[MAX_EXERCISES_EXAM]; // number of exercises for a type
EXTERN int exeqtype[MAX_EXERCISES_EXAM]; // single exercise yes or no
EXTERN int exeqput[MAX_EXERCISES_EXAM]; // check to put exercise group header
EXTERN int elemp[MAX_EXERCISES_EXAM][20][1000]; // to hold random numbers

// various arrays to hold the full exam before outputting it
EXTERN char examexertitle[MAX_EXERCISES_EXAM][MAX_STR_LENGTH]; // title
EXTERN int examexertitlech[MAX_EXERCISES_EXAM]; // check if there is a title

EXTERN int examexersingle[MAX_EXERCISES_EXAM]; // single exercise

EXTERN int examexernumst[MAX_EXERCISES_EXAM]\
     [MAX_BLOCKS_EXERCISE]; // number of statement lines
EXTERN int examexernumst2[MAX_EXERCISES_EXAM]\
     [MAX_BLOCKS_EXERCISE]; // number of statement lines
                            // this is one is used to keep the info
EXTERN char examexerstlin\
     [MAX_EXERCISES_EXAM]\
     [MAX_BLOCKS_EXERCISE]\
     [MAX_LINESTATEMENTS_EXERCISE]\
     [MAX_STR_LENGTH]; // the actual statement lines of each exercise
EXTERN	int  numexerstlin
#ifdef	MAIN_PROGRAM
=0 // number of statement lines in exercise, counter used in desc.y
#endif
;

EXTERN int examexerblocks[MAX_EXERCISES_EXAM]; // number of blocks in exercise

EXTERN int examexerbltype\
       [MAX_EXERCISES_EXAM]\
       [MAX_BLOCKS_EXERCISE]; // type of block, 1=multiplechoice,
                              // 2=shortanswer and 3=truefalse

EXTERN int exexblquto\
       [MAX_EXERCISES_EXAM]\
       [MAX_BLOCKS_EXERCISE]; // number of questions in block

EXTERN int examexernumqu[MAX_EXERCISES_EXAM]\
     [MAX_BLOCKS_EXERCISE]\
     [MAX_QUESTIONS_BLOCK]; // number of question lines
EXTERN char examexerqulin\
     [MAX_EXERCISES_EXAM]\
     [MAX_BLOCKS_EXERCISE]\
     [MAX_QUESTIONS_BLOCK]\
     [MAX_LINESTATEMENTS_QUESTION]\
     [MAX_STR_LENGTH]; // the actual question lines of each exercise

EXTERN int examexernuman[MAX_EXERCISES_EXAM]\
     [MAX_BLOCKS_EXERCISE]\
     [MAX_QUESTIONS_BLOCK]; // number of answer lines
EXTERN char examexeranlin\
     [MAX_EXERCISES_EXAM]\
     [MAX_BLOCKS_EXERCISE]\
     [MAX_QUESTIONS_BLOCK]\
     [MAX_LINESTATEMENTS_ANSWER]\
     [MAX_STR_LENGTH]; // the actual answer lines of each exercise
EXTERN	int  numexerqulin
#ifdef	MAIN_PROGRAM
=0 // number of question lines in exercise, counter used in exer.y
#endif
;EXTERN	int  numexeranlin
#ifdef	MAIN_PROGRAM
=0 // number of answer lines in exercise, counter used in exer.y
#endif
;
EXTERN int exerblockqueanch[MAX_EXERCISES_EXAM]\
     [MAX_BLOCKS_EXERCISE]\
     [MAX_QUESTIONS_BLOCK]; // to check if there is an answer at all

EXTERN int examexerblockquetotopt\
       [MAX_EXERCISES_EXAM]\
       [MAX_BLOCKS_EXERCISE]\
       [MAX_QUESTIONS_BLOCK]; // number of options in multiplechoiec
EXTERN int examexerblockquecorr\
       [MAX_EXERCISES_EXAM]\
       [MAX_BLOCKS_EXERCISE]\
       [MAX_QUESTIONS_BLOCK]; // correct answer in question 
EXTERN char examexerblockqueopt\
       [MAX_EXERCISES_EXAM]\
       [MAX_BLOCKS_EXERCISE]\
       [MAX_QUESTIONS_BLOCK]\
       [MAX_OPTIONS_QUESTION]\
       [MAX_STR_LENGTH]; // the actual options in multiplechoice question								   

