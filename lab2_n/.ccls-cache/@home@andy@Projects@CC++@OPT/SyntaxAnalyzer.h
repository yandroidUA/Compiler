#pragma once
#include "LexerResult.h"
#include <vector>
#include "Tree.h"
#include <functional>

class SyntaxAnalyzer {
public:
	SyntaxAnalyzer(std::vector<LexerResult>&);

	// start analyzing
	void analyze();

	// dunping tree into file
	void dumpTreeIntoFile(std::string&, std::string);

	// return poitner to tree
	Tree* getResultTree();

private:
	Tree tree;
	bool errorSyntaxHappened = false;
	std::vector<LexerResult> lexerResults;
	std::string errorMessageSyntax;

	// returns item or nullptr from lexerResults
	LexerResult* getItem(int);

	// set errorSyntaxHappened to TRUE and print error
	void handleError(const char*, LexerResult*);

	// analyze first string of program, returns false if error happened
	bool caseProgram();

	// analyze 3. <block> --> <variable-declarations> BEGIN <statements-list> END, starting with index, that passed as parametr
	LexerResult* caseBlock(int);

	// analyze 9. <range> --> <unsigned-integer> .. <unsigned-integer>, starting with index, that passed as parametr
	LexerResult* caseRange(int);

	// analyze 13. <variable> --> <variable-identifier><dimension>, that passed as parametr
	LexerResult* caseVariable(int);

	// analyze 14. <dimension> --> [ <expression> ] | <empty>, starting with index, that passed as parametr
	LexerResult* caseDimension(int);

	// analyze 12. <expression> --> <variable> | <unsigned-integer>, starting with index, that passed as parametr
	LexerResult* caseExpression(int);

	// analyze 7. <attributes-list> --> <attribute> <attributes-list> | <empty>, starting with index, that passed as parametr
	LexerResult* caseAttributeList(int);

	// analyze 10. <statements-list> --> <statement> <statements-list> | <empty>, starting with index, that passed as parametr
	LexerResult* caseStatementList(int);

	// analyze 5. <declarations-list> --> <declaration><declarations-list>|<empty>, starting with index, that passed as parametr
	LexerResult* caseDeclarationList(int);

	// analyze 4. <variable-declarations> --> VAR <declarations-list>|<empty>, starting with index, that passed as parametr
	LexerResult* caseVariableDeclarations(int);
	/* 
		analyze 8. <attribute> --> INTEGER | FLOAT | [<range>], starting with index, that passed as parametr
		returns pair of next item or item that don't suits case, 
		and bool that return true if item with index, that passed as parametr don't suit this case
	*/
	std::pair<LexerResult*, bool> caseAttribute(int);

	/*
		analyze 11. <statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP ;
		starting with index, that passed as parametr
		returns pair of next item or item that don't suits case,
		and bool that return true if item with index, that passed as parametr don't suit this case
	*/
	std::pair<LexerResult*, bool> caseStatement(int);

	/*
		analyze 6. <declaration> --><variable-identifier>:<attribute><attributes-list> ;
		starting with index, that passed as parametr
		returns pair of next item or item that don't suits case,
		and bool that return true if item with index, that passed as parametr don't suit this case
	*/
	std::pair<LexerResult*, bool> caseDeclaration(int);

	/*
		returns TRUE if first param is identifier
		if second param is nullptr it's add to tree cases to handle identifier
		if second param is not nullptr it's invoke second param and pass first param as param
	*/
	bool caseIdentifier(LexerResult*, std::function<void(LexerResult*)> = nullptr);

	/*
		returns TRUE if first param is variableIdentifier
		if second param is nullptr it's add to tree cases to handle identifier
		if second param is not nullptr it's invoke second param and pass first param as param
	*/
	bool caseVariableIdentifier(LexerResult*, std::function<void()> = nullptr);

};
