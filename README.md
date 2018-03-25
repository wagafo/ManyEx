# ManyEx
System to create examinations in LaTeX with permutations from a database of exercises and questions

manyex: a program to generate exams using LaTeX and Tth
=======================================================

Linux/Unix
----------

To install simply run "make" and (as root) "make install". Adjustments in the
makefile may be necessary (e.g. the INSTALLDIR variable may need to be
changed).


Examples
--------

To run a simple example, just enter the "example_simple" directory and
enter the following command line:
manyex -n -f desc-test

This will create the files "exam.tex", with the exams, "answer.tex"
formatted answers, "mn-correct" with answer key and "mn-logfile" with
a log of the operations performed.

A more complex example can be run from the example_normal directory. In
this case almost all capabilities for normal LaTeX exam creation can
be tested. See the test file description in "desc-test" and the master
LaTeX file in that directory.

An example of web form creation can be found in example_web. Please check
the files desc-test and master.sty in that subdirectory. To create
the LaTeX web form exams give the following command:
manyex -w -f desc-test

To create the html forms you need to download tth and filter the latex
output through this program. If you want to actually use the forms you
need access to a web server and knowledge on scripting to process the forms.

Shortcomings and limitations
----------------------------

Error checking is quite limited for the exercise syntax. The syntax for
exercises has to be rewritten completely to improve it, right now it works
but syntax errors in the exercise definition produce unpredictable results.
For instance commands in the execise files cannot have white space after
the command and have to start at the beginning of the line. An error
consisting of white space after a command in the exercise file is very
difficult to debug, since the program reports an error at the end of the file.

Quite large strings are defined in vars.h. If compilation is succesful but
the program fails to run most likely there is not enough memory to hold
the strings. The size can be reduced in vars.h, making sure that no exercise
exceeds these limits. String operation has to be rewritten, it works but
the program is very inefficient.
