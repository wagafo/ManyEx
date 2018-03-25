/*
  manyex: a program to produce exams from a database of exercises

  History:

  The inspiration and some code comes for the program "much":
  Copyright (C) 2004  Mihail N. Kolountzakis

  This program:
  Copyright (C) 2006-2010  Walter Garcia-Fontes

  Read the file COPYRIGHT for the complete copyright.

*/

%{

#include <stdio.h>
#include <string.h>

#include "vars.h"
#include "exer.tab.h"

int exerlex();
int exererror();
char curdir[2]=".",source[MAX_FILENAME_LENGTH], \
    dest[MAX_FILENAME_LENGTH];
void copyfile(char source[MAX_FILENAME_LENGTH], char dest[MAX_FILENAME_LENGTH]);
void random_set(int N, int k, int *elem);
void substr(char dest[], char src[], int offset, int len);

  // various int to be used below
  int qlineno=0,chline=0,chnum=0,chstat=0,  \
    fix[MAX_OPTIONS_QUESTION],numqexer=0, \
    rearrange=0, aux=0, web=0;
 
  int correct[MAX_OPTIONS_QUESTION];

  // structure to hold questions in a block of an exercise
  typedef struct block {
    char line[MAX_OPTIONS_QUESTION][10*MAX_STR_LENGTH]; // hold lines
    int correct[MAX_OPTIONS_QUESTION];                  // correct answers
    int optnum;                                         // num options
    char stquelin[MAX_LINESTATEMENTS_QUESTION]\
                [MAX_STR_LENGTH];//question statement
    int stquelinnum; // number of lines in question statement
    char stanswlin[MAX_LINESTATEMENTS_ANSWER]\
                [MAX_STR_LENGTH];//answer statement
    int stanslinnum; // number of lines in answer statement
    int checkan; // check to see if there is an answer at all
  } block;
  block myblock[MAX_QUESTIONS_BLOCK];// all info of each question of a block

  // This one is used to hold the auxiliary files, usually graphs included
  char auxfile[MAX_AUXILIARY_FILES][MAX_FILENAME_LENGTH];

  // This one is used to hold the web auxiliary files
  char webfile[MAX_AUXILIARY_FILES][MAX_FILENAME_LENGTH];

  // These are used for labeling the options within a question
  char itques[][6]={"[(A)]", "[(B)]", "[(C)]", "[(D)]",       \
		    "[(E)]", "[(F)]", "[(G)]", "[(H)]", "[(I)]", "[(J)]", \
		    "[(K)]", "[(L)]", "[(M)]", "[(N)]", "[(O)]"};

  %}

%union	{ int i; double d; char *string; }

%token <string> TITLETOK 
%token <string> AUXILIAR WEBFILE
%token <string> STRING 
%token <string> BLOCK TYPE REARRANGE 
%token <string> ENDBLOCK
%token <string> TYPE_BLOCK CHOICE 
%token <string> LEFT_BR RIGHT_BR EQUAL COMMA
%token <string> LINE_ST LINE_QU LINE_AN 
%token <string> STATEMENT ENDSTATEMENT 
%token <string> QUESTION ENDQUESTION DOT_QU COL_QU SEM_QU
%token <string> ANSWER ENDANSWER


%type <string> title_set block 
%type <string> block_set options option type_block_set choice_set 
%type <string> auxiliar_set auxiliars stringaux stringweb
%type <string> statement_set answer_set
%type <string> question_set 

%%

commands: /* empty */
|	 
commands command 
;

command:
title_set
|
auxiliar_set
|
webfile_set
|
block_set
|
endblock
|
statement_set
|
question_set
|
answer_set
;

title_set:        
TITLETOK STRING 
{
  char title[MAX_STR_LENGTH];
  strcpy(title,"{\\large ");
  strcat(title,$2);
  strcat(title,"}\n\\smallskip\n");
  strcpy(examexertitle[exer],title);
  examexertitlech[exer]=1; // mark that there is a title
}
;        

auxiliar_set:        
AUXILIAR auxiliars // copy auxiliar files to working dir
;                  

auxiliars: 
stringaux // to hold auxiliar files, written recursively (one or more)
|
auxiliars COMMA stringaux
;
// kaka
stringaux:
STRING 
{
  int check=0,i=0,j=0,ret,fold;
  DIR     *dir;
  struct dirent   *entry;
  strcpy(auxfile[aux],$1);

  // check if the file is already in the current dir, if so continue
  dir = opendir(".");  
  j=0;
  while((entry = readdir(dir)) != NULL)
    {
      ret = fnmatch(auxfile[aux], entry->d_name, 0);  
      if (!ret)
	{
	  check=1;
	  j++;
	}
    }
  // copy only if creating LaTeX normal exams
  if (!modenormal) check=1;


  // if the file is not in the current dir copy it here

  if (check==0) {
    while (i<numdir[utilcount])
      {
        int fail=0;
        dir = opendir(REL(type[utilcount][i]));
        j=0;
        while((entry = readdir(dir)) != NULL)
	  {
	    ret = fnmatch(auxfile[aux], entry->d_name, 0);  
	    if (!ret)
	      {
		strcpy(source,REL(type[utilcount][i]));
		strcat(source,"/");
		strcat(source,auxfile[aux]);              
		strcpy(dest,"./");
		strcat(dest,auxfile[aux]);
		copyfile(source,dest); 
                fail=1;}
	    j++;
	  } 
	if (fail==0){
	  char failfile[MAX_STR_LENGTH];
	  strcpy(failfile,"cannot open \"");
	  strcat(failfile, auxfile[aux]);
	  strcat(failfile, "\"\n");
	  exererror(failfile);
	  } 
        i++;
      }
  }
    aux++; check=0; j=0;
}
;

webfile_set:        
WEBFILE webfiles // copy web files to working dir
;                  

webfiles: // to hold web files, written recursively (one or more)
stringweb
|
webfiles COMMA stringweb
;

stringweb:
STRING 
{
  int check=0,i=0,j=0,ret;
  DIR     *dir;
  char curdir[2]=".",source[MAX_FILENAME_LENGTH], \
    dest[MAX_FILENAME_LENGTH];
  struct dirent   *entry;
  strcpy(webfile[web],$1);

  // check first if the file is not already in the current dir
  dir = opendir(".");  
  j=0;
  while((entry = readdir(dir)) != NULL)
    {
      ret = fnmatch(webfile[web], entry->d_name, 0);  
      if (!ret)
	{
	  check=1;
	  j++;
	}
    }  
  // copy only if creating html form exams
  if (!modeweb) check=1;

  // copy the webfile to the current dir if not already here
  if (check==0) {
  while (i<numdir[utilcount])
    {
      int fail=0;
      dir = opendir(REL(type[utilcount][i]));
      j=0;
      while((entry = readdir(dir)) != NULL)
	{
	  ret = fnmatch(webfile[web], entry->d_name, 0);
	  if (!ret)
	    {
	      strcpy(source,REL(type[utilcount][i]));
	      strcat(source,"/");
	      strcat(source,webfile[web]);
	      strcpy(dest,"./");
	      strcat(dest,webfile[web]);
	      copyfile(source,dest); 
              fail=1;}
	  j++;
	}
	if (fail==0){
	  char failfile[MAX_STR_LENGTH];
	  strcpy(failfile,"cannot open \"");
	  strcat(failfile, webfile[web]);
	  strcat(failfile, "\"\n");
	  exererror(failfile);
	  } 
      i++;
    }
  }
  web++; check=0; j=0;
}
;

// block set: arguments are optional
block_set: // set block
block
|
block LEFT_BR options RIGHT_BR
;

block:
BLOCK
{
  aux=0;
  web=0;
  // set defaults
  rearrange=1;  // rearrange by default
  type_block=1; // multiple choice by default
}
;

options:
option
|
options option
;

option:
TYPE EQUAL type_block_set
|
REARRANGE EQUAL choice_set
;

choice_set:
CHOICE
{
  if ($1) 
    {
      rearrange=1;
    }
  else
    {
      rearrange=0;
    }
}
;

type_block_set:
TYPE_BLOCK
{
  switch (type_block)
  {
 case 1:
     break;
 case 2:
     break;
 case 3:
     break;
 default:
     printf("%s\n","Syntax error in exercise file: wrong block type");
     fprintf(llog,"%s\n","Syntax error in exercise file: wrong block type");
     break;
  }
}
;

endblock:
ENDBLOCK 
{
  /* output the questions of the block */
  int i=0,j=0,elem[1000];

  examexerbltype[exer][blocknum]=type_block;

  if (rearrange == 1)
    {
      random_set(numqexer,numqexer,elem);
      exexblquto[exer][blocknum]=numqexer; // # questions block

      while (i<numqexer)
	{

          examexernumqu[exer][blocknum][i]= myblock[elem[i]].stquelinnum;
	      while (j<myblock[elem[i]].stquelinnum)
		{
		  strcpy(examexerqulin[exer][blocknum][i][j],\
			 myblock[elem[i]].stquelin[j]); 
		  j++;
		} j=0;

	  if (myblock[elem[i]].checkan==1)
	    {
	      exerblockqueanch[exer][blocknum][i]=1; // there is an answer 
	      examexernuman[exer][blocknum][i]= myblock[elem[i]].stanslinnum;
	      while (j<myblock[elem[i]].stanslinnum)
		{
		  strcpy(examexeranlin[exer][blocknum][i][j],\
			 myblock[elem[i]].stanswlin[j]);
		  j++;
		} j=0;
	    }
          examexerblockquetotopt[exer][blocknum][i]=\
                   myblock[elem[i]].optnum;
	  while (j<myblock[elem[i]].optnum)
	    {
	      char item[15];
	      strcpy(item, "\\item");
	      strcat(item, itques[j]);
	      strcat(item, " ");
              strcpy(examexerblockqueopt[exer][blocknum][i][j],\
                        myblock[elem[i]].line[j]);
	      j++;
	    } j=0;
          if (type_block==1)
	    {
	  while (j<myblock[elem[i]].optnum)
	    {
	      correct[j]=0;
	      if (myblock[elem[i]].correct[j]==1)
		{
                  examexerblockquecorr[exer][blocknum][i]=j;
		}
	      j++;
	    } j=0;
	    }
	  if (type_block==3)
	    { 
	      correct[0]=0;
	      if (myblock[elem[i]].correct[0]==0)
                  examexerblockquecorr[exer][blocknum][i]=0;
	      else
                  examexerblockquecorr[exer][blocknum][i]=1;
	    }
	  i++;
	  countques++;
	} 
    }
  else
    {
      exexblquto[exer][blocknum]=numqexer; // # questions block
      while (i<numqexer)
	{

          examexernumqu[exer][blocknum][i]= myblock[i].stquelinnum;
	      while (j<myblock[i].stquelinnum)
		{
		  strcpy(examexerqulin[exer][blocknum][i][j],\
			 myblock[i].stquelin[j]); 
		  j++;
		} j=0;

	  examexernuman[exer][blocknum][i]= myblock[i].stanslinnum;
	  if (myblock[i].checkan==1)
	    {
	      exerblockqueanch[exer][blocknum][i]=1; // there is an answer 
	      examexernuman[exer][blocknum][i]= myblock[i].stanslinnum;
	      while (j<myblock[i].stanslinnum)
		{
		  strcpy(examexeranlin[exer][blocknum][i][j],\
			 myblock[i].stanswlin[j]);
		  j++;
		} j=0;
	    }

          examexerblockquetotopt[exer][blocknum][i]=\
                   myblock[i].optnum;
	  while (j<myblock[i].optnum)
	    {
	      char item[15];
	      strcpy(item, "\\item");
	      strcat(item, itques[j]);
	      strcat(item, " ");
              strcpy(examexerblockqueopt[exer][blocknum][i][j],\
                        myblock[i].line[j]);
	      j++;
	    } j=0;
	  if (type_block==1)
	    {
	  while (j<myblock[i].optnum)
	    {
	      char item[15];
	      correct[j]=0;
	      if (myblock[i].correct[j]==1)
		{
                  examexerblockquecorr[exer][blocknum][i]=j;
		}
	      j++;
	    } j=0;
	    }
	  if (type_block==3)
	    { 
	      correct[0]=0;
	      if (myblock[i].correct[0]==0)
                  examexerblockquecorr[exer][blocknum][i]=0;
	      else
                  examexerblockquecorr[exer][blocknum][i]=1;
	    }
	  i++;
	  countques++;
	}
    }
  numqexer=0;
  rearrange=1;
  blocknum++;
}
;

statement_set:
STATEMENT lines endstatement_set
;

lines:
lines LINE_ST
{
  strcpy(examexerstlin[exer][blocknum][numexerstlin],$2);
  numexerstlin++; // number of statement lines in exercise
}
|
LINE_ST
{
  strcpy(examexerstlin[exer][blocknum][numexerstlin],$1);
  numexerstlin++; // number of statement lines in exercise
}
;

endstatement_set:
ENDSTATEMENT 
{
  examexernumst[exer][blocknum]=numexerstlin;
  examexernumst2[exer][blocknum]=numexerstlin;
  numexerstlin=0; // put statement line counter to 0
}
;

question_set:
QUESTION 
{ 
  chstat=0;
  chline=0;
  chnum=1;
  exerblockqueanch[exer]\
           [blocknum][numqexer]=0; // will change to 1 there is an answer
  myblock[numqexer].checkan=0;
}
|
DOT_QU
{
  correct[qlineno]=0;
  if (qlineno==0) 
    {
      correct[qlineno]=1;
      chline=1;
    }
  // eliminate the first two characters
  substr(temp,$1,2,strlen($1));
  strcpy(line[qlineno],temp);
  if (type_block==3) correct[qlineno]=0; // mark as true, 0=true, 1=false
  if (type_block==2) 
    {
      printf("%s\n", "Error in exercise file: Options in shortanswer question");
      fprintf(llog,"%s\n", "Error in exercise file: Options in shortanswer question");
    }
  qlineno++;
}
|
COL_QU
{
  correct[qlineno]=0;
  if (qlineno==0) 
    {
      chline=1;
    }
  substr(temp,$1,2,strlen($1));
  strcpy(line[qlineno],temp);
  fix[qlineno]=1;
  if (type_block==3) correct[qlineno]=1; // mark as false, 0=true, 1=false
  if (type_block==2) 
    {
      printf("%s\n", "Error in exercise file: Options in shortanswer question");
      fprintf(llog,"%s\n", "Error in exercise file: Options in shortanswer question");
    }
  qlineno++;
}
|
SEM_QU
{
  /* this rule for correct answer which is fixed not 
     in the first line */
  correct[qlineno]=1;
  if (qlineno!=0) correct[0]=0;
  if (qlineno==0) 
    {
      chline=1;
    }
  substr(temp,$1,2,strlen($1));
  strcpy(line[qlineno],temp);
  fix[qlineno]=1;
  if (type_block==2) 
    {
      printf("%s\n", "Error in exercise file: Options in shortanswer question");
      fprintf(llog,"%s\n", "Error in exercise file: Options in shortanswer question");
    }
  if (type_block==3) 
    {
      printf("%s\n", "Error in exercise file: Wrong option in truefalse question");
      fprintf(llog,"%s\n", "Error in exercise file: Wrong option in truefalse question");
    }
  qlineno++;
}
|
LINE_QU
{
  if (chline==0) 
    { 
      /* copy the statement of question to the block holder */
  strcpy(myblock[numqexer].stquelin[numexerqulin],$1);
  numexerqulin++; // number of question lines in exercise
    }
  /* append extra lines to options */
  else 
    {           
      strcat(line[qlineno-1], " ");
      strcat(line[qlineno-1], $1);
    }        
}
|
ENDQUESTION 
{
  int i=0, j=0, k=0, elem[1000];
  random_set(qlineno,qlineno,elem);
  strcpy(myblock[numqexer].stquelin[numexerqulin],$1);
  myblock[numqexer].stquelinnum=numexerqulin;
  numexerqulin=0; // put question line counter to 0

  while (i<qlineno)
    {
      if (fix[i]==1 & elem[i] != i)
	{
	  k=elem[i];
	  while (j<qlineno)
	    {
	      if (elem[j]==i) elem[j]=k;
	      j++;
	    }
	  elem[i] = i;
	  j=0;
	}
      i++;
    }
           
  j=0;
  /* copy the question to the block holder */
  myblock[numqexer].optnum = qlineno;
  while (j< qlineno) 
    {
      strcpy(myblock[numqexer].line[j],line[elem[j]]);        
      myblock[numqexer].correct[j]=correct[elem[j]];
      fix[j]=0;
      j++;
    }

  qlineno=0;
  chline=0;
  chstat=0;
  numqexer++;
} 
       
;

answer_set:
ANSWER lines_an ENDANSWER
{
  myblock[numqexer].checkan=1;
  exerblockqueanch[exer][blocknum][numqexer]=0; 
  strcpy(myblock[numqexer].stanswlin[numexeranlin],$1);
  myblock[numqexer].stanslinnum=numexeranlin;
  numexeranlin=0; // put answer line counter to 0
}
;

lines_an:
line_an
|
lines_an line_an
;

line_an:
LINE_AN
{
  strcpy(myblock[numqexer].stanswlin[numexeranlin],$1);
  numexeranlin++; // number of answer lines in exercise
}
;

%%

int exererror(char *s) {
  fprintf(llog, "Line %d of %s: %s\n", exerlineno, temp2, s); 
  fprintf(stderr, "Line %d of %s : %s\n", exerlineno, temp2, s);
}
  

