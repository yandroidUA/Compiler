#pragma once
#include "LexerResult.h"
#include <vector>

class SyntaxAnalyzer
{
public:
	SyntaxAnalyzer(std::vector<LexerResult>&);
	void analyze();
private:
	int currentIndex = 0;
	std::vector<LexerResult> lexerResults;

	// reads from lexerResults symbol with index @param
	LexerResult getItem(int);
	
	void handleError(const char*, LexerResult);
 
	bool caseProgram(int);
	LexerResult caseIdentifier(int);
	LexerResult caseBlock(int);
	LexerResult caseVariableDeclarations(int);
	LexerResult caseDeclarationList(int);
	LexerResult caseDeclaration(int);
	LexerResult caseAttribute(int);
	LexerResult caseAttributeList(int);
	LexerResult caseRange(int);
	// index of currentItem and index of token to end recurssion
	LexerResult caseStatementList(int, int);
	LexerResult caseStatement(int);
	LexerResult caseExpression(int);
	LexerResult caseDimension(int);
	LexerResult caseVariable(int);

};


/*
1. <signal-program> --> <program>
2. <program> --> PROGRAM <procedure-identifier> ;
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
