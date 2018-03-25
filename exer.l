/*
  manyex: a program to produce exams from a database of exercises

  History:

  The inspiration and some code comes for the program "much":
  Copyright (C) 2004  Mihail N. Kolountzakis

  This program:
  Copyright (C) 2006-2010  Walter Garcia-Fontes

  Read the file COPYRIGHT for the complete copyright.

*/

%x state_env quest_env aquest_env

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vars.h"
#include "exer.tab.h"  

%}

%option yylineno
%option prefix="exer"

start_state     "statement"
end_state       "endstatement"
start_quest     "question"
end_quest       "endquestion"
start_aquest    "answer"
end_aquest      "endanswer"
line             (.*)
blank_line       \n
dot_qu           (\.)(.*)
col_qu           (\:)(.*)
sem_qu           (\;)(.*)
%%
"title"              return TITLETOK;
"auxiliar"           return AUXILIAR;
"webfile"            return WEBFILE;
"block"              return BLOCK;
"endblock"           return ENDBLOCK;
\(                   return LEFT_BR;
\)                   return RIGHT_BR;
\,                   return COMMA;
"type"               return TYPE;
"rearrange"          return REARRANGE;
multiplechoice|shortanswer|truefalse   {
                     if (!strcmp(exertext,"multiplechoice")) 
                             type_block=1;
                     else if (!strcmp(exertext,"shortanswer"))
                             type_block=2; 
                          else if (!strcmp(exertext,"truefalse"))
                             type_block=3;
                               else type_block=0;
                     return TYPE_BLOCK;}
yes|no               {exerlval.i =! strcmp(exertext,"yes"); return CHOICE;}
"="                  return EQUAL;
\"[^\"\n]*\"         {exerlval.string= strdup(exertext)+1;exerlval.string[strlen(exerlval.string)-1]=0;return STRING;}
<INITIAL,quest_env,aquest_env>{blank_line}   /* ignore EOL but not in state */;
<INITIAL>[ \t]+         /* ignore whitespace but not in envs*/;
<INITIAL>{start_state}       {BEGIN(state_env); return STATEMENT;}
<state_env>{end_state}       {BEGIN(INITIAL); return ENDSTATEMENT;}
<state_env>{line}             {exerlval.string=strdup(exertext);return LINE_ST;}
<state_env>{blank_line}       {exerlval.string=strdup(exertext);return LINE_ST;}
<INITIAL>{start_quest}        {BEGIN(quest_env);return QUESTION;}
<quest_env>{start_aquest}     {BEGIN(aquest_env); return ANSWER;}
<aquest_env>{end_aquest}      {BEGIN(quest_env); return ENDANSWER;}
<aquest_env>{line}            {exerlval.string=strdup(exertext); return LINE_AN;}
<quest_env>{end_quest}        {BEGIN(INITIAL); return ENDQUESTION;}
<quest_env>{dot_qu}           {exerlval.string=strdup(exertext);return DOT_QU;}
<quest_env>{col_qu}           {exerlval.string=strdup(exertext);return COL_QU;}
<quest_env>{sem_qu}           {exerlval.string=strdup(exertext);return SEM_QU;}
<quest_env>{line}             {exerlval.string=strdup(exertext);return LINE_QU;}

<*>"#"[^\n]*		/* eat up one-line comments starting with # */;
\r                      ; /* ignore DOS linefeeds */
%%
exerwrap()
{ return(1); }

