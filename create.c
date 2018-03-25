/*
  manyex: a program to produce exams from a database of exercises

  History:

  The inspiration and some code comes for the program "much":
  Copyright (C) 2004  Mihail N. Kolountzakis

  This program:
  Copyright (C) 2006-2010  Walter Garcia-Fontes

  Read the file COPYRIGHT for the complete copyright.

  Subroutines to create exams. 
*/

#include	"vars.h"

int paper, count=1,l=0,m=0,n=0, hold=0;

void write_preamble_normal ( FILE *wri )
{
// Subroutine to write the preamble of the exam file

  // Write the preamble of the exam file

  // This is written at the beginning of exam file (or files if web)
  fprintf(wri,"\\documentclass[%s]{article}\n",documentclassoptions);
  // Include packages at the beginning of exam file (or files if web)
  if (strcmp(usepackage," "))
  fprintf(wri,"\\usepackage{%s}\n",usepackage);  

  // include the master package files

  fprintf(wri,"\\usepackage{%s}\n",mastertex);

  // Write the begin document command
  fprintf(wri,"%s\n","\\pagestyle{empty}");  
  fprintf(wri,"%s\n","\\begin{document}");  
  // print the header macro in the exam latex file
  fprintf(wri,"%s\n","\\mnglobalheader");
}

void write_preamble_web ( FILE *wri )
{
// Subroutine to write the preamble of the exam file

  // Write the preamble of the exam file

  // This is written at the beginning of exam file (or files if web)
  fprintf(wri,"\\documentclass[%s]{article}\n",documentclassoptions);

  // Include packages at the beginning of exam file (or files if web)

  // include the master package files
      char extension[10];
      strcpy(extension,mastertex);
      strcat(extension,".sty");
      fprintf(wri,"\\input{%s}\n",extension);

  // Write the begin document command
  fprintf(wri,"%s\n","\\begin{document}");  

  // print the header macro in the exam latex file
  fprintf(wri,"%s\n","\\mnglobalheader");

  // Print a title for the html file
      fprintf(wri,"%s","%%tth:\\begin{html} <title> ");
      fprintf(wri,"%s",pagetitle);
      fprintf(wri,"%s\n", "</title> \\end{html}");    


}

void choose_exercise()
{
  // This subroutine chooses the exercises of the type chosen
  // and put the name of the files in exerfile[]


  DIR     *dir;
  struct dirent   *entry;
  char   name[MAX_STR_LENGTH];
  int    ret, exist=0;

  int elem[1000],\
    available,needed,q,check;

  // to hold exercise files to be parsed, large enough to hold long path if 
  // needed
  char exerfile[MAX_FILES_BROWSE+50][MAX_FILENAME_LENGTH];
  
   // utility string holder
  char temp3[MAX_STR_LENGTH];


      while(i<numtype)
	{
           m=0;j=0;
          
	  while (m<numdir[i])
	    {
	      strcpy(temp2,basedir);
	      strcat(temp2,"/");
	      strcat(temp2,type[i][m]);
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
		      strcpy(temp3, entry->d_name); 
                      strcpy(exerfile[j], temp2);
	          strcat(exerfile[j],"/");
                  strcat(exerfile[j], temp3);
                      j++;
		    }
		} k=0;

		  k=j;
	      m++;
	    } 
          m=0;k=0;

	  k=0;
	  available=j; hold=chran;
          needed=numqtype[i];

          if (available==needed) {
              int r=0;
              random_set(available,needed, elem);        
              while (r<1000){
                elemp[i][m][r]=elem[r];
		r++; 
	      } r=0;
	  } 

	  /*	  else if (available>=needed) */
	  if (available>=needed)
	    {
              check=0;
              while(k<needed)
		{
		  q=k+chran; hold=q+1;
		  if (q>available-1) {
                     q=chran+k-available; hold=q+1;
		  }
		  exerin = fopen(exerfile[elemp[i][m][q]], "r");
		  if (!exerin) {
		    fprintf(stderr, "Cannot open exercise file %s: ",\
                               exerfile[elemp[i][m][q]]);
		    perror("");
		    exit(1);
		  }

		  countques=1;
                  fprintf(llog,"Exercise used %s\n",exerfile[elemp[i][m][q]]);
		  utilcount=i; // pass the type number to exer.y for auxiliars
	          examexersingle[exer]=exeqtype[i];
                  strcpy(temp2,exerfile[elemp[i][m][q]]); //pass to exer.y for error
                  examexertitlech[exer]=0;
         	  exerparse();  
                  exerlineno=1;
		  fclose(exerin);
                  examexerblocks[exer]=blocknum;
                  blocknum=0;
 		  k++; 
		  if (examexersingle[exer]==0) realexer++;
		  exer++; 
		} 
	    }
	  else {
	    printf("Not enough files for type %s\n",type[i]); 
	    }
	  i++; 
	} chran=hold;
}

// Subroutine to output normal LaTex exam
void output_normal_exam()
{
      // Output a complete exam
      i=0;j=0;k=0;l=0;
      int exercount=1;
      fprintf(out,"\\def\\mnserialnumber{%i}\n",paper);

      fprintf(out,"%s\n","\\mnpaperheader");

      while (i<exer)
        {
          if ((((examexersingle[i]==1) & (i==0)) || \
	      (examexersingle[i]==0)) & (realexer>0))
	    {          
	  fprintf(out,"\\def\\mnexercisenumber{%i}\n",exercount);
	  fprintf(out,"%s","\\mnexerciseheader ");
          exercount++;
	    }
          if (exeqput[i]) fprintf(out,"%s","\\mnexercisestartheader\n");

          if (examexertitlech[i]) fprintf(out,"%s",examexertitle[i]);

          while (j<examexerblocks[i])
	    {

	      while (k<examexernumst[i][j])
		{
		  fprintf(out,"%s",examexerstlin[i][j][k]);
		  k++;
		}
	      fprintf(out,"%s\n\n","\\smallskip");
              examexernumst[i][j]=0; //otherwise statement appears later
	      k=0;

              while (k<exexblquto[i][j])
		{ 
         	  fprintf(out,"\\def\\mnquestionnumber{%i}\n",count);
	          fprintf(out,"%s","\\mnquestionheader ");

		      while (l<examexernumqu[i][j][k])
			{
			  fprintf(out,"%s\n",examexerqulin[i][j][k][l]);
			  l++;
			} l=0;


		      if (examexerbltype[i][j]==1) 
		    {
		      fprintf(out,"\\begin{itemize}\n"); 
		      while (l<examexerblockquetotopt[i][j][k])
			{
			  char item[15];
			  char itques[][6]={"[(A)]","[(B)]","[(C)]","[(D)]", \
					    "[(E)]","[(F)]","[(G)]","[(H)]", \
                                            "[(I)]","[(J)]","[(K)]","[(L)]", \
                                            "[(M)]", "[(N)]", "[(O)]"};
			  strcpy(item, "\\item");
			  strcat(item, itques[l]);
			  strcat(item, " ");
			  fprintf(out,"%s",item); 
			  fprintf(out,"%s\n",\
				  examexerblockqueopt[i][j][k][l]);
			  l++;
			} l=0;
		      fprintf(out,"%s\n","\\end{itemize}"); 
		    }
		      

		      if (examexerbltype[i][j]==3) 
		    {
		      fprintf(out,"%s","\\framebox[0.5in]{\\phantom{T}}\\qquad "); 
			  fprintf(out,"%s\n",\
				  examexerblockqueopt[i][j][k][0]);
		    }

        	  fprintf(out,"%s\n\n","\\mnquestionfooter"); 
        	  k++;
                  count++;
		} k=0;
	      j++;
	    } 
          j=0;
          if ((((examexersingle[i]==1) & (i==0)) || \
	      (examexersingle[i]==0)) & (realexer>0))
	    {          
	  fprintf(out,"%s","\\mnexercisefooter\n\n");
	    }
          count=1;
	  i++;
	} i=0;

      fprintf(out,"%s\n","\\mnpaperfooter");
      fprintf(out,"%s\n\n","\\pagebreak");
}

// Subroutine to output a complete web exam
void output_web_exam()
{
      // Output a complete exam
      i=0;j=0;k=0;l=0;
      fprintf(outw,"\\def\\mnserialnumber{%i}\n",paper);
      fprintf(outw,"%s\n","\\mnpaperheader");

      // This is a javascript function to get an uncheck button in web multiplechoice questions
      fprintf(outw,"%s\n","%%tth:\\begin{html} <HEAD> \\end{html}");
      fprintf(outw,"%s\n","%%tth:\\begin{html} <SCRIPT> \\end{html}");
      fprintf(outw,"%s","%%tth:\\begin{html} function uncheckRadioGroup (radioButtonOrGroup) {");
      fprintf(outw,"%s","if (radioButtonOrGroup.length) {");
      fprintf(outw,"%s","for (var b = 0; b < radioButtonOrGroup.length; b++)");
      fprintf(outw,"%s","if (radioButtonOrGroup[b].checked) {");
      fprintf(outw,"%s","radioButtonOrGroup[b].checked = false;");
      fprintf(outw,"%s","break;");
      fprintf(outw,"%s","}");
      fprintf(outw,"%s","}");
      fprintf(outw,"%s","else ");
      fprintf(outw,"%s","radioButtonOrGroup.checked = false;");
      fprintf(outw,"%s\n","} \\end{html}"); 
      fprintf(outw,"%s\n","%%tth:\\begin{html} </SCRIPT> \\end{html}");
      fprintf(outw,"%s\n","%%tth:\\begin{html} </HEAD> \\end{html}");


      fprintf(outw,"%s\n","\\mnformheader");

      // write the code for the form with correct answers
      fprintf(outw,"%s","%%tth:\\begin{html}<FORM action=\"");
      fprintf(outw,"%s",actionfield);
      fprintf(outw,"%s","?perm=");
      fprintf(outw,"%i",exam);

      if (solution==1)  // only write solutions to form if wanted
	{
      i=0; count=0; 
      while (i<exer)
	{
	  while (j<examexerblocks[i])
	    {
	      while (k<exexblquto[i][j])
		{
		      char itques[][6]={"A", "B", "C", "D", \
                      "E", "F", "G", "H", "I", "J", \
                      "K", "L", "M", "N", "O"};
		  fprintf(outw,"%s","&c");
		  fprintf(outw, "%i",count);
		  fprintf(outw,"%s","=");
                  fprintf(outw,"%s",\
               itques[examexerblockquecorr[i][j][k]]); 
		  k++;
                  count++;
		}
	      k=0;
	      j++;
	    }
	  j=0;
	  i++;
	}
	}
      fprintf(outw,"%s\n","\" method=\"post\">\\end{html}");

      i=0;j=0;k=0; numrequired=0;
      while (i<inifieldnum)
	{
      // write the initial fields for the form
 
      fprintf(outw,"\n%s\n", textfield[i]);
      fprintf(outw,"%s\n","%%tth:\\begin{html}    <P>\\end{html}");
      fprintf(outw,"%s","%%tth:\\begin{html}    <LABEL for=\"");
      fprintf(outw,"%s", namefield[i]);
      fprintf(outw,"%s","\">");
      fprintf(outw,"%s", labelfield[i]);
      fprintf(outw,"%s\n", "</LABEL>\\end{html}");
      fprintf(outw,"%s","%%tth:\\begin{html} <INPUT type=\"");
      fprintf(outw,"%s", typefield[i]);
      fprintf(outw,"%s","\" name=\"");
      fprintf(outw,"%s", namefield[i]);
      fprintf(outw,"%s", "\" size=");
      fprintf(outw,"%s", sizefield[i]);
      fprintf(outw,"%s\n", "><BR>\\end{html}");
      if (fieldrequired[i]==1)
	{
	  strcpy(printfield[numrequired],fieldname[i]);
	  strcpy(printtextreq[numrequired],textrequired[i]);
	  numrequired++;
	} 
      i++;
        } i=0;
      
      fprintf(outw,"%s\n","\\mnformfooter");


      fprintf(outw,"%s\n","\\bigskip");


      i=0;j=0;k=0;count=1;
      int exercount=1;
      while (i<exer)
        {
	  // write the beginning of an exercise

          if ((((examexersingle[i]==1) & (i==0)) || \
	       (examexersingle[i]==0)) & (realexer>0))
	    {          
	  fprintf(outw,"\\def\\mnexercisenumber{%i}\n",exercount);
	  fprintf(outw,"%s","\\mnexerciseheader ");
          exercount++;
	    }
          if (exeqput[i]) fprintf(outw,"%s","\\mnexercisestartheader\n");
	  fprintf(outw,"%s\n",examexertitle[i]);

	  // write the questions themselves
          while (j<examexerblocks[i])
	    {

	      while (k<examexernumst[i][j])
		{
		  fprintf(outw,"%s",examexerstlin[i][j][k]);
		  k++;
		}
	      fprintf(outw,"%s\n\n","\\smallskip");
              examexernumst[i][j]=0; //otherwise statement appears later
	      k=0;

              while (k<exexblquto[i][j])
		{ 
         	  fprintf(outw,"\\def\\mnquestionnumber{%i}\n",count);
	          fprintf(outw,"%s","\\mnquestionheader ");

		      while (l<examexernumqu[i][j][k])
			{
			  fprintf(outw,"%s\n",examexerqulin[i][j][k][l]);
			  l++;
			} l=0;

                  fprintf(outw,"\n%s\n","\\medskip");

                  if (examexerbltype[i][j]==1) 
		    {
		      fprintf(outw,"%s","\\begin{html} <INPUT TYPE=\"button\" VALUE=\"");
                      fprintf(outw,"%s",uncheck);
                      fprintf(outw, "%s\n", "\" \\end{html}");
                      fprintf(outw,"%s", "\\begin{html} ONCLICK=\"uncheckRadioGroup(this.form.ex");
                      fprintf(outw,"%i",count);
                      fprintf(outw,"%s\n",")\"> \\end{html}");
        	  fprintf(outw,"\\begin{itemize}\n"); 
                  while (l<examexerblockquetotopt[i][j][k])
			{
		      char item[15];
		      char itques[][6]={"A", "B", "C", "D",\
                      "E", "F", "G", "H", "I", "J", \
                      "K", "L", "M", "N", "O"};
                      fprintf(outw,"%s","%%tth:\\begin{html}<INPUT type=\"radio\" name=\"ex");
                      fprintf(outw,"%i",count);
                      fprintf(outw,"%s","\" value=\"");
                      fprintf(outw,"%s",itques[l]);
                      fprintf(outw,"%s","\"> ");
                      fprintf(outw,"(%s) ",itques[l]);
                      fprintf(outw,"%s\n","\\end{html}");
                      fprintf(outw,"%s",examexerblockqueopt[i][j][k][l]);
                      fprintf(outw,"%s","%%tth;\\begin{html}");
                      fprintf(outw,"%s\n\n"," <BR>\\end{html}");
		      l++;
		    } l=0;
         	  fprintf(outw,"%s\n","\\end{itemize}"); 
		    }


                  if (examexerbltype[i][j]==2) 
		    {
                     fprintf(outw,"%s","%%tth:\\begin{html}<TEXTAREA name=\"ex");
                     fprintf(outw,"%i",k);
                     fprintf(outw,"%s","\" rows=\"10\" cols=\"80\"> </TEXTAREA> <BR> \\end{html}");
		    }

                  if (examexerbltype[i][j]==3) 
		    {
                      fprintf(outw,"%s\n","%%tth:\\begin{html}<SELECT  size=\"1\" name=\"truefalse\"> \\end{html}");
                      fprintf(outw,"%%%%tth:\\begin{html}<OPTION value=\"choose\">%s</OPTION>\\end{html}\n", label_tf[0]);
                      fprintf(outw,"%%%%tth:\\begin{html}<OPTION value=\"true\">%s</OPTION>\\end{html}\n", label_tf[1]);
                      fprintf(outw,"%%%%tth:\\begin{html}<OPTION value=\"false\">%s</OPTION>\\end{html}\n", label_tf[2]);
                      fprintf(outw,"%s\n","%%tth:\\begin{html}</SELECT>\\end{html}");
			  fprintf(outw,"%s\n",\
				  examexerblockqueopt[i][j][k][0]);
		    }


        	  fprintf(outw,"%s\n\n","\\mnquestionfooter"); 
        	  k++;
                  count++;
		} k=0;
	      j++;
	    } j=0;
          if ((((examexersingle[i]==1) & (i==0)) || \
	      (examexersingle[i]==0)) & (realexer>0))
	    {          
	  fprintf(outw,"%s","\\mnexercisefooter\n\n");
	    }
          count=1;

	  i++;
	} i=0;
      // write required fields warnings if any, don't write them in the answer file
      if (checkrequired==1)
	{
          i=0;
	  fprintf(outw, "%s\n", "%%tth:\\begin{html} <script type=\"text/javascript\"> \\end{html}");
	  fprintf(outw, "%s", "%%tth:\\begin{html} window.onload=function(){");
          fprintf(outw, "%s", "var elForm=document.getElementsByTagName('form')[0];");
	  fprintf(outw, "%s", " elForm.onsubmit=function() ");
          fprintf(outw, "%s", "{ ");
          fprintf(outw, "%s", " var required=['");
          while (i<numrequired)
	    {
	      fprintf(outw, "%s", printfield[i]);
              if (i<numrequired-1)
		{
		  fprintf(outw, "%s", "','");
		}
	      i++;
	    } i=0;
	  fprintf(outw, "%s", "']; ");
          fprintf(outw, "%s", " var textreq=['");
          while (i<numrequired)
	    {
	      fprintf(outw, "%s", printtextreq[i]);
              if (i<numrequired-1)
		{
		  fprintf(outw, "%s", "','");
		}
	      i++;
	    } i=0;
	  fprintf(outw, "%s", "'];");
	  fprintf(outw, "%s", " var bool=true; ");
          fprintf(outw, "%s", " for(var i=0;i<required.length;i++) ");
          fprintf(outw, "%s", " { ");
          fprintf(outw, "%s", " if(document.getElementsByName(required[i])[0].value=='') ");
          fprintf(outw, "%s", "{ ");
	  fprintf(outw, "%s", "alert(textreq[i]); ");
          fprintf(outw, "%s", " bool=false; ");
          fprintf(outw, "%s", "} ");
          fprintf(outw, "%s", "} ");
          fprintf(outw, "%s", " return bool; ");
          fprintf(outw, "%s", " } ");
          fprintf(outw, "%s\n", " } \\end{html}");
	  fprintf(outw, "%s\n", "%%tth:\\begin{html} </script> \\end{html}");
	}

      // write the submit information, don't write it in the answer file
      fprintf(outw,"%s\n", "\\mnsubmitheader");

      fprintf(outw,"%s", "%%tth:\\begin{html}    <INPUT type=\"submit\" value=\"");
      fprintf(outw,"%s", textsubmit);
      fprintf(outw,"%s\n", "\"> \\end{html}");
      fprintf(outw,"%s\n","%%tth:\\begin{html}    </P>\\end{html}");

      // write the end of the exann file
      fprintf(outw,"%s\n\n","\\mnpaperfooter");
      fprintf(outw,"%s\n","\\end{document}");  
      numrequired=0;

} 

// Subroutine to output normal LaTex answer
void output_normal_answer()
{
      // Output a complete normal answer latex file
      i=0;j=0;k=0;l=0;
      int exercount=1;
      fprintf(ans,"\\def\\mnserialnumber{%i}\n",paper);
      fprintf(ans, "%s\n", "\\mnanswerpaperheader"); // answer paper header
      fprintf(ans,"%s\n","\\mnpaperheader");

      while (i<exer)
        {
          if ((((examexersingle[i]==1) & (i==0)) || \
	       (examexersingle[i]==0)) & (realexer>0))
	    {          
	  fprintf(ans,"\\def\\mnexercisenumber{%i}\n",exercount);
	  fprintf(ans,"%s","\\mnexerciseheader ");
	  exercount++;
	    }
          if (exeqput[i]) fprintf(ans,"%s","\\mnexercisestartheader\n");
	  if (examexertitlech[i]) fprintf(ans,"%s",examexertitle[i]);

          while (j<examexerblocks[i])
	    {

              examexernumst[i][j] = examexernumst2[i][j];
	      while (k<examexernumst[i][j])
		{
		  fprintf(ans,"%s",examexerstlin[i][j][k]);
		  k++;
		}
	      fprintf(ans,"%s\n\n","\\smallskip");
              examexernumst[i][j]=0; //otherwise statement appears later
              examexernumst2[i][j]=0; //otherwise statement appears later
	      k=0;

              while (k<exexblquto[i][j])
		{ 
         	  fprintf(ans,"\\def\\mnquestionnumber{%i}\n",count);
	          fprintf(ans,"%s","\\mnquestionheader ");

		      while (l<examexernumqu[i][j][k])
			{
			  fprintf(ans,"%s\n",examexerqulin[i][j][k][l]);
			  l++;
			} l=0;

                  if (examexerbltype[i][j]==1) 
		    {
        	  fprintf(ans,"\\begin{itemize}\n"); 
                  while (l<examexerblockquetotopt[i][j][k])
		    {
		      char item[15], item2[15], item3[20];
		      char itques[][6]={"[(A)]", "[(B)]", "[(C)]", "[(D)]",\
                      "[(E)]", "[(F)]", "[(G)]", "[(H)]", "[(I)]", "[(J)]", \
                      "[(K)]", "[(L)]", "[(M)]", "[(N)]", "[(O)]"};
		      strcpy(item, "\\item");
		      strcat(item, itques[l]);
                      substr(item2,itques[l],2,1);
		      strcat(item, " ");

		      if (l == examexerblockquecorr[i][j][k])
			{
                          strcpy(item3,"\\item[");
			  strcat(item3,"\\fbox{(");
			  strcat(item3,item2);
			  strcat(item3,")}]");
			  strcat(item3," ");
			  fprintf(ans,"%s",item3); 
			}
                      else
                        {
                          fprintf(ans,"%s",item); 
			}
                      fprintf(ans,"%s\n",\
	 		      examexerblockqueopt[i][j][k][l]);
		      l++;
		    } l=0; 
         	  fprintf(ans,"%s\n","\\end{itemize}"); 
		    }

                      if (examexerbltype[i][j]==3) 
		    {
		      if (examexerblockquecorr[i][j][k]==0)
		      fprintf(ans,"\\framebox{%s}\\qquad ",\
			      label_tf[1]); 
                      else
		      fprintf(ans,"\\framebox{%s}\\qquad ",\
                              label_tf[2]); 
			  fprintf(ans,"%s\n",\
				  examexerblockqueopt[i][j][k][0]);
		    }


        	  fprintf(ans,"%s\n\n","\\mnquestionfooter"); 

		  if (exerblockqueanch[i][j][k]==1)
		    {
		      fprintf(ans,"%s\n", "\\mnanswerheader");
		      while (l<examexernuman[i][j][k])
			{
			  fprintf(ans,"%s\n",examexeranlin[i][j][k][l]);
			  l++;
			}  l=0;
		      fprintf(ans,"%s\n", "\\mnanswerfooter");
		      fprintf(ans,"\n");
		    } 
        	  k++;
                  count++;
		} k=0;
	      j++;
	    } j=0;
          if ((((examexersingle[i]==1) & (i==0)) || \
	      (examexersingle[i]==0)) & (realexer>0))
	    {          
	  fprintf(ans,"%s","\\mnexercisefooter\n\n");
	    }
          count=1;
	  i++;
	} i=0;
      fprintf(ans,"%s\n\n","\\mnpaperfooter");
      fprintf(ans,"%s\n\n","\\pagebreak");
}

// Subroutine to output web answers (one file for each exam
void output_web_answer()
{
      // Output a complete answer
      i=0;j=0;k=0;l=0;
      int exercount=1;

      // output beginning of answer files
      fprintf(answ,"\\def\\mnserialnumber{%i}\n",paper);
      fprintf(answ, "%s\n", "\\mnanswerpaperheader");
      fprintf(answ,"%s\n","\\mnpaperheader");

      fprintf(answ,"%s\n","\\mnformheader");

      // write the code for the form with correct answers
      fprintf(answ,"%s","%%tth:\\begin{html}<FORM >\\end{html}");

      i=0;j=0;k=0;

      // write the initial fields for the answer file
      while (i<inifieldnum)
	{
      // write the initial fields for the form
 
      fprintf(answ,"\n%s\n", textfield[i]);
      fprintf(answ,"%s\n","%%tth:\\begin{html}    <P>\\end{html}");
      fprintf(answ,"%s","%%tth:\\begin{html}    <LABEL for=\"");
      fprintf(answ,"%s", namefield[i]);
      fprintf(answ,"%s","\">");
      fprintf(answ,"%s", labelfield[i]);
      fprintf(answ,"%s\n", "</LABEL>\\end{html}");
      fprintf(answ,"%s","%%tth:\\begin{html} <INPUT type=\"");
      fprintf(answ,"%s", typefield[i]);
      fprintf(answ,"%s","\" name=\"");
      fprintf(answ,"%s", namefield[i]);
      fprintf(answ,"%s", "\" size=");
      fprintf(answ,"%s", sizefield[i]);
      fprintf(answ,"%s\n", "><BR>\\end{html}");
      i++;
        }
      
      fprintf(answ,"%s\n","\\mnformfooter");


      fprintf(answ,"%s\n","\\bigskip");


      i=0;j=0;k=0;count=1;
      while (i<exer)
        {
	  // write the beginning of an exercise in the answer file
          if ((((examexersingle[i]==1) & (i==0)) || \
	       (examexersingle[i]==0)) & (realexer>0))
	    {          
	  fprintf(answ,"\\def\\mnexercisenumber{%i}\n",exercount);
	  fprintf(answ,"%s","\\mnexerciseheader ");
	  exercount++;
	    }
          if (exeqput[i]) fprintf(answ,"%s","\\mnexercisestartheader\n");
	  fprintf(answ,"%s\n",examexertitle[i]);

	  // write the questions themselves in the answer file
          while (j<examexerblocks[i])
	    {

              examexernumst[i][j] = examexernumst2[i][j];
	      while (k<examexernumst[i][j])
		{
		  fprintf(answ,"%s",examexerstlin[i][j][k]);
		  k++;
		}
	      fprintf(answ,"%s\n\n","\\smallskip");
              examexernumst[i][j]=0; //otherwise statement appears later
              examexernumst2[i][j]=0; //otherwise statement appears later
	      k=0;

              while (k<exexblquto[i][j])
		{ 
         	  fprintf(answ,"\\def\\mnquestionnumber{%i}\n",count);
	          fprintf(answ,"%s","\\mnquestionheader ");

		      while (l<examexernumqu[i][j][k])
			{
			  fprintf(answ,"%s\n",examexerqulin[i][j][k][l]);
			  l++;
			} l=0;

                      if (examexerbltype[i][j]==1) 
			{
        	  fprintf(answ,"\\begin{itemize}\n"); 
                  while (l<examexerblockquetotopt[i][j][k])
		    {
		      char item[15];
		      char itques[][6]={"A", "B", "C", "D",\
                      "E", "F", "G", "H", "I", "J", \
                      "K", "L", "M", "N", "O"};
                      fprintf(answ,"%s","%%tth:\\begin{html}<INPUT type=\"radio\" name=\"ex");
                      fprintf(answ,"%i",count); 
                      fprintf(answ,"%s","\" value=\"");
                      fprintf(answ,"%s",itques[l]);


		      if (l == examexerblockquecorr[i][j][k])
			{
                      fprintf(answ,"%s","\" checked> ");
                      fprintf(answ,"(%s) ",itques[l]);
			}
                      else
                        {
                      fprintf(answ,"%s","\"> ");
                      fprintf(answ,"(%s) ",itques[l]);
			}
                      fprintf(answ,"%s\n","\\end{html}");
                      fprintf(answ,"%s",examexerblockqueopt[i][j][k][l]);
                      fprintf(answ,"%s","%%tth;\\begin{html}");
                      fprintf(answ,"%s\n\n"," <BR>\\end{html}");
		      l++;
		    } l=0;
         	  fprintf(answ,"%s\n","\\end{itemize}"); 
			}



                      if (examexerbltype[i][j]==3) 
		    {
		      if (examexerblockquecorr[i][j][k]==0)
		      fprintf(answ,"\\framebox{%s} \\qquad ",\
			      label_tf[1]); 
                      else
		      fprintf(answ,"\\framebox{%s} \\qquad ",\
                                     label_tf[2]); 
			  fprintf(answ,"%s\n",\
				  examexerblockqueopt[i][j][k][0]);
		    }


        	  fprintf(answ,"%s\n\n","\\mnquestionfooter"); 

		  if (exerblockqueanch[i][j][k]==1)
		    {
		      fprintf(answ,"%s\n", "\\mnanswerheader");
		      while (l<examexernuman[i][j][k])
			{
			  fprintf(answ,"%s\n",examexeranlin[i][j][k][l]);
			  l++;
			} l=0;
		      fprintf(answ,"%s\n", "\\mnanswerfooter");
		      fprintf(answ,"\n");
                    } 
        	  k++;
                  count++;
		} k=0;
	      j++;
	    } j=0;
          if ((((examexersingle[i]==1) & (i==0)) || \
	      (examexersingle[i]==0)) & (realexer>0))
	    {          
	  fprintf(answ,"%s","\\mnexercisefooter\n\n");
	    }
          count=1;	  

	  i++;
	} i=0;

      // write the end of the answer file
      fprintf(answ,"%s\n\n","\\mnpaperfooter");
      fprintf(answ,"%s\n","\\end{document}");  

}

// Output the answer key
void output_answer_key()
{
      i=0;j=0;k=0;l=0;

      while (i<exer)
        {
          while (j<examexerblocks[i])
	    {
              while (k<exexblquto[i][j])
		{ 
		      char itques[][6]={"A", "B", "C", "D", \
                      "E", "F", "G", "H", "I", "J", \
                      "K", "L", "M", "N", "O"};
                  fprintf(corr,"%s",\
               itques[examexerblockquecorr[i][j][k]]); 
        	  k++;
		} k=0;
	      j++;
	    } j=0;
	  i++;
	} i=0;
}
// soubroutine create output
void create_output(int actionnormal,int actionanswer,\
               int actionweb,int actionwebanswer, int actionkey)
{

  if (actionnormal){
  // open the file to write the exams 
  strcat(examtex,".tex");
  out = fopen(examtex, "w");
  if (!out) {
    fprintf(stderr, "Cannot open output exam file: ");
    perror("");
    exit(1);
  }

  // Write the preamble of the normal LaTeX exam
  write_preamble_normal(out);
  }

  if (actionanswer){
  // open the file to write the answers
  strcat(answertex,".tex");
  ans = fopen(answertex, "w");
  if (!ans) {
    fprintf(stderr, "Cannot open answer exam file: ");
    perror("");
    exit(1);
  }

  // Write the preamble of the normal answer LaTeX file
  write_preamble_normal(ans);
  }

  if (actionkey){
  // open the file to write the answer key
  corr = fopen("mn-correct","w");
  if (!corr) {
    fprintf(stderr, "Cannot open answer key file: ");
    perror("");
    exit(1);
  }

  }

  while (exam < numexam)
    {
      // In this subroutine the exercises of the whole exam are chosen
      // and then the complete exam is output to the appropiate files

      int itemp;
      char str[10];
      char examweb[MAX_FILENAME_LENGTH];
      char answeb[MAX_FILENAME_LENGTH];

      // convert integer exam to string
      itemp = sprintf(str, "%i", exam);

 
      if (actionweb){
  // open the files to write the exams
  strcpy(examweb,examtex);
  strcat(examweb,str);
  strcat(examweb,".tex");
  outw = fopen(examweb, "w");
  if (!outw) {
    fprintf(stderr, "Cannot open output exam file: ");
    perror("");
    exit(1);
  }

  write_preamble_web(outw);
      }

      if (actionwebanswer){
  // open the files to write the answers
  strcpy(answeb,answertex);
  strcat(answeb, str);
  strcat(answeb,".tex");
  answ= fopen(answeb, "w");
  if (!answ) {
    fprintf(stderr, "Cannot open answer exam file: ");
    perror("");
    exit(1);
  }

  write_preamble_web(answ);
      }
    
      exer=0;
      paper=exam+iniserial;
      fprintf(llog,"Starting serial number %i\n",paper);

      if (actionkey){
      fprintf(corr,"%.3i",paper);
      }
    
      i=0;

      choose_exercise();
                  checkdirw++;
                  if (checkdirw>numdir[i-1]-1) checkdirw=0;
      if (actionnormal){
      output_normal_exam();
      }

      if (actionweb){
	output_web_exam();
        fclose(outw); 
      }
        
      if (actionanswer){
	output_normal_answer();
      }

      if (actionwebanswer){
	output_web_answer();
        fclose(answ);
      }

      if (actionkey){
	output_answer_key();
        fprintf(corr,"\n"); 
        }

      exam++;
    } exam=0; 


  // Write the end document command

  if (actionnormal){
  fprintf(out,"%s\n","\\end{document}");  
  fclose(out); 
  }

  if (actionanswer){
  fprintf(ans,"%s\n","\\end{document}");  
  fclose(ans); 
  }

  if (actionkey){
  fclose(corr);
  }
}
