BASE = manyex# base name
BIN  = $(BASE)
CC   = gcc
HEADERS = exer.tab.h desc.tab.h vars.h
OBJS = $(BASE).o lex.exer.o lex.desc.o randomlib.o \
       exer.tab.o desc.tab.o create.o util.o
LEX = flex
YACC = bison
INSTALLDIR = /usr/local/bin
MANDIR = /usr/local/man/man1
CFLAGS = -g
LIBS = -lm  

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(BIN)

$(BASE).o:	$(BASE).c $(HEADERS)

create.o:  create.c $(HEADERS)

util.o:     util.c $(HEADERS)

randomlib.o:	randomlib.c randomlib.h $(HEADERS)

lex.exer.o:	lex.exer.c $(HEADERS)

lex.desc.o:	lex.desc.c $(HEADERS)

exer.tab.o:	exer.tab.c $(HEADERS)

desc.tab.o:	desc.tab.c $(HEADERS)

exer.tab.h exer.tab.c: exer.y
	$(YACC) -d -p exer exer.y 

desc.tab.h desc.tab.c: desc.y
	$(YACC) -d -p desc desc.y 

lex.exer.c: exer.l exer.tab.h
	$(LEX) exer.l 

lex.desc.c: desc.l desc.tab.h
	$(LEX) desc.l 

all:    
	-touch exer.y exer.l desc.y desc.l manyex.c \
         randomlib.c util.c create.c *.h make $(BIN)

install: # $(BIN) man/manyex.1
	cp $(BIN) $(INSTALLDIR)
	cp man/manyex.1 $(MANDIR)

clean:
	-rm -f $(OBJS) $(BIN) *.tab.* lex.* *.exe \
            example_simple/*.tex example_simple/mn-* \
            example_simple/exam.* example_simple/answer.* \
            example_normal/mn-* \
            example_normal/exam.* example_normal/answer.* \
            example_normal/*.eps example_normal/mn-*  \
            example_web/exam*.* example_web/answer*.* \
            example_web/*.jpg example_web/*.html example_web/mn-* \
            doc/manyex.info
	rm -fr doc/manyex
	rm -fr doc/auto
	rm -fr example_simple/auto
	rm -fr example_normal/auto
	rm -fr example_web/auto




