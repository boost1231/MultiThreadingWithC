# Stuff
VPATH = bin

.PHONY: all
all : bin echoclient echoserver

echoclient : csapp.o EchoClientMain.c
	clang -o bin/echoclient bin/csapp.o EchoClientMain.c

echoserver : echo.o csapp.o EchoServerMain.c
	clang -o bin/echoserver echo.c bin/csapp.o EchoServerMain.c

csapp.o : csapp.h csapp.c
	clang -o bin/csapp.o -c csapp.c

bin :
	mkdir -p bin

.PHONY: clean
clean :
	rm -rf bin
