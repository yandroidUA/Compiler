
CC=g++
EXECUTABLE=andy_yan
CFLAGS=-c -Wall

.PHONY: help build run clean

help:
	$(info make run ARG='PATH_TO_SOURCE' - to compile and run program)
	$(info make build - to compile program, executable will be a $(EXECUTABLE))
	$(info clean - to clean generated files)

#./${EXECUTABLE} will be generated
build: clean launcher
	$(CC) -Wall -g -o $(EXECUTABLE) *.o

#compile and start ARG='PATH_TO_SOURCE
run: build
	./$(EXECUTABLE) $(ARG)

tree: Tree.cpp Tree.h Rules.h LexerResult.h
	$(CC) $(CFLAGS) Tree.cpp

lexer: Lexer.cpp ReservedWords.h MultiSeparatedToken.h LexerResult.h TokenStatus.h
	$(CC) $(CFLAGS) Lexer.cpp

syntax_analyzer: SyntaxAnalyzer.cpp tree Rules.h ReservedWords.h LexerResult.h
	$(CC) $(CFLAGS) SyntaxAnalyzer.cpp

translator: tree Translator.cpp Translator.h ReservedWords.h Identifier.h Attribute.h RangeAttribute.h Expression.h
	$(CC) $(CFLAGS) Translator.cpp

#compiles everything for launcher
launcher: lexer syntax_analyzer translator
	$(CC) $(CFLAGS) Launcher.cpp

clean:
	rm -rvf *.o
	rm -rvf $(EXECUTABLE)
