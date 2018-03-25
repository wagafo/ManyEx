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

#define	MAIN_PROGRAM

#include "desc.tab.h"
#include "vars.h"

  int singleput=0;
int desclex();
int yylex();
void random_set(int N, int k, int *elem);
int descerror();

%}

%union	{ int i; char *p; }

%token	<i>INTEGER
%token	<p>ID
%token	<p>STRING
%token	LEFT_PARENTHESIS RIGHT_PARENTHESIS
%token  DIRECTORY DOCUMENTCLASSOPTIONS USEPACKAGE 
%token  MASTERTEX EXAMTEX ANSWERTEX EXERPREFIX

%token  FORMACTION SOLUTION INIFIELD NAME LABEL TYPE SIZE TEXT PAGETITLE 
%token  UNCHECK TEXTSUBMIT REQUIREFIELD
%token  LEFT_BR RIGHT_BR EQUAL COMMA

%token	USE FROM SEED SERIAL CREATE TRUEFALSE ARGUMENT
%token  EXERCISE ENDEXERCISE 

%start input
%%


input:
	  /* Empty */
	| input statement
	;

statement:
          DIRECTORY STRING 
	  {
	    strcpy(basedir, $2);
	    fprintf(llog,"%d: base dir set to %s\n", desclineno, basedir);
 	  }
        | DOCUMENTCLASSOPTIONS STRING 
	  {
	    strcpy(documentclassoptions, $2);
	    fprintf(llog,"%d: documentclass options set to %s\n", desclineno, documentclassoptions);
 	  }
        | MASTERTEX STRING 
	  {
	    strcpy(mastertex, $2);
	    fprintf(llog,"%d: master latex file to be included %s.tex\n", desclineno, mastertex);
 	  }
        | USEPACKAGE STRING 
	  {
	    strcpy(usepackage, $2);
	    fprintf(llog,"%d: packages to be used %s\n", desclineno, usepackage);
 	  }
        | EXAMTEX STRING 
	  {
	    strcpy(examtex, $2);
	    fprintf(llog,"%d: exam(s) file(s) to be created with prefix %s\n", desclineno, examtex);
 	  }
        | ANSWERTEX STRING 
	  {
	    strcpy(answertex, $2);
	    fprintf(llog,"%d: answer(s) file(s) to be created with prefix %s\n", desclineno, answertex);
 	  }
          | EXERPREFIX STRING
          {
            strcpy(exerprefix, $2);
	    fprintf(llog,"%d: Exercise prefix set to %s\n", desclineno, exerprefix);
	  }
          | PAGETITLE STRING
          {
            strcpy(pagetitle, $2);
	    fprintf(llog,"%d: Page title set to %s\n", desclineno, pagetitle);
          }
          | FORMACTION STRING 
          {
            strcpy(actionfield, $2);
	    fprintf(llog,"%d: action set to %s\n", desclineno, actionfield);
            solution=0;
          }
          | FORMACTION SOLUTION STRING 
          {
            strcpy(actionfield, $3);
	    fprintf(llog,"%d: action (with solutions) set to %s\n", desclineno, actionfield);
            solution=1;
          }
        | INIFIELD LEFT_BR NAME EQUAL STRING LABEL EQUAL STRING TYPE EQUAL STRING SIZE EQUAL STRING RIGHT_BR
	  {
	    strcpy(namefield[inifieldnum], $5);
            strcpy(labelfield[inifieldnum], $8);
            strcpy(typefield[inifieldnum], $11);
            strcpy(sizefield[inifieldnum], $14);
     	   inifieldnum++;            
	  }
        | INIFIELD LEFT_BR NAME EQUAL STRING LABEL EQUAL STRING TYPE EQUAL STRING SIZE EQUAL STRING TEXT EQUAL STRING RIGHT_BR
	  {
            char newl[3];
	    strcpy(namefield[inifieldnum], $5);
            strcpy(labelfield[inifieldnum], $8);
            strcpy(typefield[inifieldnum], $11);
            strcpy(sizefield[inifieldnum], $14);
            strcpy(textfield[inifieldnum], $17);
     	   inifieldnum++;            
	  }
        | INIFIELD LEFT_BR NAME EQUAL STRING LABEL EQUAL STRING TYPE EQUAL STRING SIZE EQUAL STRING REQUIREFIELD EQUAL STRING RIGHT_BR
	  {
            char newl[3];
	    strcpy(namefield[inifieldnum], $5);
            strcpy(labelfield[inifieldnum], $8);
            strcpy(typefield[inifieldnum], $11);
            strcpy(sizefield[inifieldnum], $14);
            strcpy(textrequired[inifieldnum], $17);
            strcpy(fieldname[inifieldnum], $5);
            checkrequired=1;
            fieldrequired[inifieldnum]=1;
            numrequired++;
     	   inifieldnum++;            
	  }
        | INIFIELD LEFT_BR NAME EQUAL STRING LABEL EQUAL STRING TYPE EQUAL STRING SIZE EQUAL STRING TEXT EQUAL STRING REQUIREFIELD EQUAL STRING RIGHT_BR
	  {
            char newl[3];
	    strcpy(namefield[inifieldnum], $5);
            strcpy(labelfield[inifieldnum], $8);
            strcpy(typefield[inifieldnum], $11);
            strcpy(sizefield[inifieldnum], $14);
            strcpy(textfield[inifieldnum], $17);
            strcpy(textrequired[inifieldnum], $20);
            strcpy(fieldname[inifieldnum], $5);
            checkrequired=1;
            fieldrequired[inifieldnum]=1;
            numrequired++;
     	   inifieldnum++;            
	  }
        | UNCHECK STRING
          {
            strcpy(uncheck, $2);         
	    fprintf(llog,"%d: unchek set to %s\n", desclineno, uncheck);
          }
        | TEXTSUBMIT STRING
          {
            strcpy(textsubmit, $2);
	    fprintf(llog,"%d: textsubmit set to %s\n", desclineno, textsubmit);
          }
	| SEED INTEGER 
	  {
	    seed = $2;
	    RandomInitialise(seed, seed);       
	    /* RandomInitialise(seed, seed); */
	    fprintf(llog,"%d: seed set to %d, random number generator initialized\n", desclineno, seed);
	  }
        | truefalse_set
        | EXERCISE
          {
            singleexer=1; // between EXERCISE and ENDEXERCISE consider a 
	                  // single exercise
            singleput=1;
            exeqput[i]=singleput; // mark to put exercise group header
            singleput=0;
          }
	| use_set
        | ENDEXERCISE
          {
            singleexer=0;
          }
	| SERIAL INTEGER 
	  {
            iniserial=$2;
	  }
	| CREATE INTEGER 
	  {
            numexam=$2;
	  }
        ;

truefalse_set:  
       TRUEFALSE STRING COMMA STRING COMMA STRING
       {
	 strcpy(label_tf[0],$2);
	 strcpy(label_tf[1],$4);
	 strcpy(label_tf[2],$6);
       }
       ;

use_set:
	choose_exer
        |
	choose_exer ARGUMENT
        ;

choose_exer:
	USE INTEGER FROM strings
          {
	    k=0;
            numqtype[i]=$2; // number of exercises of this type
            exeqtype[i]=singleexer; //remember if single exercise
            j=j+$2;            
            numexer=j;      // total number of exercises
            i++;
            numtype=i;      // number of different types
          }
        ;

	 
strings:
          STRING
          { 
             DIR     *dir;
             struct dirent   *entry;
            int elem[1000],l=0, m=0, ret;
            char   name[MAX_STR_LENGTH];
            strcpy(type[i][k],$1);  // dir k to browse of type i-1

	      strcpy(temp2,basedir);
	      strcat(temp2,"/");
	      strcat(temp2,type[i][k]);
	      dir = opendir(temp2);  

	      if (!dir) {
		fprintf(stderr, "Cannot open base directory: \n");
                fprintf(stderr, "%s\n", temp2);
		perror("");
		exit(1);
	      }

              strcpy(name,exerprefix);
	      strcat(name,"*");

	      while((entry = readdir(dir)) != NULL)
		{
		  ret = fnmatch(name, entry->d_name, 0);  
		  if (!ret)
		    {
                      l++;
		    }
		}

              random_set(l,l, elem);
              while (m<1000){
                elemp[i][k][m]=elem[m];
		m++;
	      } m=0; 
            k++;
            numdir[i]=k;   // total number of dirs of this type
	  }        
        | strings COMMA STRING
          {
             DIR     *dir;
             struct dirent   *entry;
            int elem[1000],l=0, m=0, ret;
            char   name[MAX_STR_LENGTH];

            strcpy(type[i][k],$3);

	      strcpy(temp2,basedir);
	      strcat(temp2,"/");
	      strcat(temp2,type[i][k]);
	      dir = opendir(temp2);  

	      if (!dir) {
		fprintf(stderr, "Cannot open base directory: \n");
                fprintf(stderr, "%s\n", temp2);
		perror("");
		exit(1);
	      }

              strcpy(name,exerprefix);
	      strcat(name,"*");

	      while((entry = readdir(dir)) != NULL)
		{
		  ret = fnmatch(name, entry->d_name, 0);  
		  if (!ret)
		    {
                      l++;
		    }
		}
              random_set(l,l, elem);
              while (m<1000){
                elemp[i][k][m]=elem[m];
		m++; 
	      } m=0;
	        k++; 
            numdir[i]=k;   // total number of dirs of this type
 	  }
        ;

%%

int descerror(char *s) {
  fprintf(llog, "Line %d of test descripction file: %s\n", desclineno, s);
  fprintf(stderr, "Line %d of test description file: %s\n", desclineno, s);
}
