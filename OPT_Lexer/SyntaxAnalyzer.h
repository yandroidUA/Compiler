#pragma once
#include "LexerResult.h"
#include <vector>
#include "Tree.h"

class SyntaxAnalyzer
{
public:
	SyntaxAnalyzer(std::vector<LexerResult>&);
	void analyze(const char*);
	void dumpTreeIntoFile(const char*, std::string);
private:
	int currentIndex = 0;
	std::vector<LexerResult> lexerResults;
	Tree tree;

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

/*									RULES
1. <signal-program> --> <program>
2. <program> --> PROGRAM <procedure-identifier>;
<block>.
3. <block> --> <variable-declarations> BEGIN <statements-list> END
4. <variable-declarations> --> VAR <declarations-list>|<empty>
5. <declarations-list> --> <declaration><declarations-list>|<empty>
6. <declaration> --><variable-identifier>:<attribute><attributes-list> ;
7. <attributes-list> --> <attribute> <attributes-list> | <empty>
8. <attribute> --> INTEGER | FLOAT | [<range>]
9. <range> --> <unsigned-integer> .. <unsigned-integer>
10. <statements-list> --> <statement> <statements-list> | <empty>
11. <statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP ;
12. <expression> --> <variable> | <unsigned-integer>
13. <variable> --> <variable-identifier><dimension>
14. <dimension> --> [ <expression> ] | <empty>
15. <variable-identifier> --> <identifier>
16. <procedure-identifier> --> <identifier>
----------------------------------------------------------------------------------------------
17. <identifier> --> <letter><string>
18. <string> --> <letter><string> | <digit><string> | <empty>
19. <unsigned-integer> --> <digit><digits-string>
20. <digits-string> --> <digit><digits-string> | <empty>
21. <digit> --> 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
22. <letter> --> A | B | C | D | ... | Z
*/
