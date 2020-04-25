#pragma once
#include "LexerResult.h"
#include <vector>
#include "Tree.h"

class SyntaxAnalyzer
{
public:
	SyntaxAnalyzer(std::vector<LexerResult>&);
	void analyze();
	void dumpTreeIntoFile(std::string&, std::string);
	Tree* getResultTree();
private:
	int currentIndex = 0;
	std::vector<LexerResult> lexerResults;
	Tree tree;
	// for error handling
	bool errorSyntaxHappened = false;
	std::string errorMessageSyntax;

	/*	reads from lexerResults symbol with index @param 
		if IndexOutOfBoundsException -> print exception "Unexpected end of file"
		and end analyzing
	*/
	LexerResult getItem(int);
	// 
	void handleError(const char*, LexerResult);
	/*	start analyzing with 2-nd rule 
		return true if exception not hapenned and false if happened 
		during analyzing all program
	*/
	bool caseProgram(int);
	/*	each of this functions takes int
		this is index of item in lexerResults to analyze
		and each of this function return item next from lexerResults
		that don't match rule of function
		or LexerResult nullableResult("", -1, -1, -1, -1); */
	LexerResult caseBlock(int);
	LexerResult caseRange(int);
	LexerResult caseVariable(int);
	LexerResult caseVariableIdentifier(int);
	LexerResult caseAttribute(int, Tree::TreeItem*);
	LexerResult caseStatement(int);
	LexerResult caseDimension(int);
	LexerResult caseIdentifier(int);
	LexerResult caseExpression(int);
	LexerResult caseDeclaration(int);
	LexerResult caseAttributeList(int);
	LexerResult caseDeclarationList(int, Tree::TreeItem *);
	LexerResult caseVariableDeclarations(int);
	/*	this function takes 2 parametrs,
	first one is default as for other functions
	second is code of item
	fucntion check code of item that have index of first param and if it is equal second param
	return this item in other case continue it logic */
	LexerResult caseStatementList(int, int);

};
