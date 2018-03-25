/*
  manyex: a program to produce exams from a database of exercises

  History:

  The inspiration and some code comes for the program "much":
  Copyright (C) 2004  Mihail N. Kolountzakis

  This program:
  Copyright (C) 2006-2014  Walter Garcia-Fontes

  Read the file COPYRIGHT for the complete copyright.

  Main program and subroutines.
*/

#include	"vars.h"

static char *SCCSid = "@(#)manyex V1.0, " __DATE__ "\n";
const char* appname;

// help or print this if no arguments are given
void print_use ()
{
  printf ("%s\n", SCCSid + 4);
  printf ("Copyright 2006-2014 Walter Garcia-Fontes\n");
  printf ("This program comes with NO WARRANTY, to the extent permitted by law.\n");
  printf ("You may redistribute it under the terms of the GNU General Public License;\n");
  printf ("see the file named COPYING for details.\n");
  printf ("Written by Walter Garcia-Fontes.\n");
  printf("Use: %s [options] -f test-description\n", appname);
  printf(" -h  --help                    Show this help\n"
         " -n  --normal                  Create normal LaTeX exams and answer (default)\n"
	 " -w  --webform                 Create webform exams and answers\n"
         " -a  --normal-answer           Create only normal LaTeX answer\n"
         " -o  --form-anwer              Create only web form answer\n"
         " -s  --suppress-answer         Supress latex answers (with -n)\n"
         " -t  --supress-webanswer       Supress web answer (with -w)\n"
         " -k  --supress-key             Supress answer key\n"
         " -f --file \"test description file\"   Specify test description file (required)\n");
}

int main (int argc, char **argv)
{

  int next_option;
  int actionnormal=0;
  int actionweb=0;
  int actionanswer=0;
  int actionwebanswer=0;
  int actionkey=1; // always create answer key unless supressed
  int supressanswer=0, supresswebanswer=0, supresskey=0;
  char descfile[MAX_FILENAME_LENGTH];
  int foption=0;
 
  /* Save program name to include when we exit */
  appname = argv[0];

  while (1)
    {

      /* Array describing correct long options */
      static struct option op_long[] =
	{
	  { "help",        no_argument,  0,   'h'},
	  { "normal",      no_argument,  0,   'n'},
	  { "webform",     no_argument,  0,   'w'},
	  { "description-file",  required_argument,  0, 'f'},
          { "normal-answer", no_argument, 0, 'a'},
          { "webform-answer", no_argument, 0, 'o'},
          { "supress-answer", no_argument, 0, 's'},
          { "supress-webanswer", no_argument, 0, 't'},
          { "supress-answerkey", no_argument, 0, 'k'},
	  { 0, 0, 0, 0}
	};
  
      int option_index =0;

      /* We call the getopt functon */
      next_option = getopt_long (argc, argv, "hnwaostkf:", op_long, &option_index);
    
      /* Detect the end of the options */


      if (next_option == -1)
        {
	  if (argc==1) 
	    {
	      printf("%s\n", "You have to give at least -f \"test description file\"");
	      printf("%s\n", "to create latex normal exam and answer.");
	      printf("\n");
	      print_use();
	      exit(0);
	    }
	  break; /* No more options, go out from this loop */
	}
      switch (next_option)
	{

	case 'h': /* -h or --help */
	  print_use();
	  exit(0);
        
	case 'n': /* -n o --normal */
	  actionnormal=1;  // produce normal LaTeX exams
          actionanswer=1;  // produce normal LaTeX answers
	  break;
        
	case 'w': /* -w ó --webform */
	  actionweb=1;       // produce webform exams
          actionwebanswer=1; // produce web answers
	  break;

	case 'a':
          actionanswer=1;  
	  break;

	case 'o':
          actionwebanswer=1; 
          break; 

	case 's':
          supressanswer=1;  
	  break;

	case 't':
          supresswebanswer=1; 
	  break;

	case 'k':
	  supresskey=1;
	  break;
        
	case 'f': /* give test-description file */
	  strcpy(descfile,optarg); 
          foption=1;
	  break;

	case '?': /* invalid option */
	  print_use(); 
	  exit(0);

	default: /* Something else? Don't wait, abort */
	  abort();
	}
    }



  /* Print any remaining command line arguments (not options). */
 
  if (optind < argc)
    {
      printf ("Wrong option given: ");
      while (optind < argc)
	printf ("%s ", argv[optind++]);
      putchar ('\n');
      printf("\n");
      print_use();
      exit(0);
    }

  if (supressanswer) actionanswer=0;
  if (supresswebanswer) actionwebanswer=0;
  if (supresskey) actionkey=0;

  if (foption==0)
    { 
      printf("%s\n", "You have to specify -f \"test description file\".");
      exit(0);
    }	  

  // default if no options given is to write normal exam and answer
  if ((actionnormal==0) && (actionweb==0)&& (actionanswer==0) \
      && (actionwebanswer==0)) 
    {
      actionnormal=1;
      actionanswer=1;
    }

  //open log file
  llog = fopen("mn-logfile", "w"); 
  if (!llog) {
    fprintf(stderr, "Cannot open log file: ");
    perror("");
    exit(1);
  }
  // parse the text-description file
  descin = fopen(descfile, "r");
  if (!descin) {
    fprintf(stderr, "Cannot open test description file: ");
    perror("");
    exit(1);
  }

  // some defaults for descparse
  strcpy(pagetitle,"Title");
  strcpy(uncheck, "Uncheck");

  descparse(); 
  fclose(descin);

  // set which type of output we are creating
  modenormal=0;
  modeweb=0;
  if ((actionnormal) || (actionanswer)) modenormal=1;
  if ((actionweb) || (actionwebanswer)) modeweb=1;

create_output(actionnormal,actionanswer,\
	      actionweb,actionwebanswer,actionkey);

  fclose(llog);

 end:

  exit(0);
}
