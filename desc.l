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
#include <stdlib.h>
#include <string.h>

#include "vars.h"
#include "desc.tab.h"  

%}

%option yylineno
%option prefix="desc"


WHITE			[ \t\n]+
LETTER                  [A-Za-z]
DIGIT                   [0-9]
INTEGER                 {DIGIT}+
INTOREMPTY              {DIGIT}*
ID                      {LETTER}({LETTER}|{DIGIT})*
%%
"#"[^\n]*		{} // eat up one-line comments starting with #
{WHITE}			{}
"directory"		{ return(DIRECTORY); }
"exerprefix"            { return EXERPREFIX; }
"documentclassoptions"  { return DOCUMENTCLASSOPTIONS; }
"usepackage"            { return USEPACKAGE; }
"mastertex"             { return MASTERTEX;  }
"examtex"               { return EXAMTEX;  }
"answertex"             { return ANSWERTEX; }
"pagetitle"             { return PAGETITLE; }
"formaction"            { return FORMACTION; }
"solution"              { return SOLUTION; }
"inifield"              { return INIFIELD; }
"name"                  { return NAME; }
"type"                  { return TYPE; }
"size"                  { return SIZE; }
"label"                 { return LABEL; }
"text"                  { return TEXT;}
"uncheck"               { return UNCHECK;}
"textsubmit"            { return TEXTSUBMIT; }
"requirefield"          { return REQUIREFIELD; }
"truefalse"             { return TRUEFALSE; }
"argument"              { return ARGUMENT; }
"use"			{ return USE; }
"exercise"              { return EXERCISE; }
"endexercise"           { return ENDEXERCISE; }
"seed"			{ return SEED; }
"serial"		{ return SERIAL; }
"create"		{ return CREATE; }
"from"   		{ return FROM; }
{INTEGER}	 	{ desclval.i = atoi(desctext); return INTEGER; } 
{ID}			{ desclval.p = strdup(desctext); return ID; }
\"[^\"\n]*\"		{ desclval.p = strdup(desctext)+1; desclval.p[strlen(desclval.p)-1]=0; return STRING; } 

\(                   { return LEFT_BR; }
\)                   { return RIGHT_BR; }
"="                  { return EQUAL; }
\,                   { return COMMA; }
\r                   /* eat up DOS line endings */;
%%
descwrap()
{ return(1); }
