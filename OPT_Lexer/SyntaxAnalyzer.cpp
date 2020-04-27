#include "SyntaxAnalyzer.h"
#include <fstream>
#include "Rules.h"
#include "ReservedWords.h"

LexerResult nullableResult("", -1, -1, -1, -1);

SyntaxAnalyzer::SyntaxAnalyzer(std::vector<LexerResult>& res) {
	this->lexerResults = res;
	this->errorMessageSyntax = "";
	this->errorSyntaxHappened = false;
}

void SyntaxAnalyzer::analyze(){  
	tree.addChild("<signal-program> --> <program>", -1, -1, SIGNAL_PROGRAM, -1);
	if (caseProgram(0)) {
		// tree.print();
	}
	tree.print();
}

void SyntaxAnalyzer::dumpTreeIntoFile(std::string& filename, std::string startText) {
	std::ofstream file;
	file.open(filename);
	file << startText << std::endl << std::endl;

	if (errorSyntaxHappened) {
		file << errorMessageSyntax << std::endl << std::endl;
	}

	tree.dumpIntoFile(file);
	file.close();
}

Tree* SyntaxAnalyzer::getResultTree() {
	return &tree;
}

LexerResult SyntaxAnalyzer::getItem(int index){
	if (lexerResults.size() <= index) {
		errorSyntaxHappened = true;
		std::cout << "Unexpected end of file" << std::endl;
		errorMessageSyntax = "Unexpected end of file";
		return nullableResult;
	}
	return lexerResults.at(index);
}

void SyntaxAnalyzer::handleError(const char* message, LexerResult failedItem) {
	errorSyntaxHappened = true;
	std::cout << message << ", row = " << failedItem.getRowNumber() << " column = " << failedItem.getColumnNumber() << ", but got " << failedItem.getToken() << std::endl;
	errorMessageSyntax = message;
	errorMessageSyntax.append(", row = ");
	errorMessageSyntax.append(std::to_string(failedItem.getRowNumber()));
	errorMessageSyntax.append(" column = ");
	errorMessageSyntax.append(std::to_string(failedItem.getColumnNumber()));
	errorMessageSyntax.append(" but got ");
	errorMessageSyntax.append(failedItem.getToken());
}

// 2. < program > --> PROGRAM <procedure - identifier>;
bool SyntaxAnalyzer::caseProgram(int index) {
	Tree::TreeItem* it = tree.addNext("<program> --> PROGRAM <procedure-identifier>;< block > .", -1, -1, PROGRAM_RULE, -1);
	
	if (getItem(index).getCode() != PROGRAM) {
		handleError("PROGRAM expected", getItem(index));
		return false;
	}

	tree.addNext("<procedure-identifier> --> <identifier>", -1, -1, PROCEDURE_IDENTIFIER, -1);
	LexerResult nextItem = caseIdentifier(index + 1);
	tree.switchTo(it);

	if (errorSyntaxHappened) return false;

	if (nextItem.getCode() != SEMI_COLON) {
		handleError("; expected", nextItem);
		return false;
	}
	tree.addNext("<block> --> <variable-declarations> BEGIN <statements-list> END", -1, -1, BLOCK, -1);
	nextItem = caseBlock(nextItem.getIndexInResultVector() + 1);
	return !errorSyntaxHappened;
}

// 3. < block > --> <variable - declarations> BEGIN <statements - list> END
LexerResult SyntaxAnalyzer::caseBlock(int index) {
	Tree::TreeItem* current = tree.getCurrent();
	LexerResult item = caseVariableDeclarations(index);

	tree.switchTo(current);
	if (errorSyntaxHappened) return nullableResult;

	if (item.getCode() != BEGIN) {
		handleError("BEGIN expected", item);
		return nullableResult;
	}

	tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_RESERVED_WORD, BEGIN);
	item = caseStatementList(item.getIndexInResultVector() + 1, END);

	tree.switchTo(current);
	if (errorSyntaxHappened) return nullableResult;

	if (item.getCode() != END) {
		handleError("END expected", item);
		return nullableResult;
	}

	tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_RESERVED_WORD, item.getCode());
	return item;
}

// 4. <variable-declarations> --> VAR <declarations-list>|<empty>
LexerResult SyntaxAnalyzer::caseVariableDeclarations(int index) {
	Tree::TreeItem* it = tree.getCurrent();
	tree.addNext("<variable-declarations> --> VAR <declarations-list>|<empty>", -1, -1, VARIABLE_DECLARATIONS, -1);
	LexerResult item = getItem(index);

	if (item.getCode() == BEGIN) {
		tree.addChild("<empty>", -1, -1, EMPTY, -1);
		return item;
	}

	if (item.getCode() != VAR) {
		handleError("VAR or <empty> expected", getItem(index));
		return nullableResult;
	}
	tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_RESERVED_WORD, item.getCode());
	return caseDeclarationList(item.getIndexInResultVector() + 1, it);
}

// 5. <declarations-list> --> <declaration><declarations-list>|<empty>
LexerResult SyntaxAnalyzer::caseDeclarationList(int index, Tree::TreeItem* root) {
	tree.addNext("<declarations-list> --> <declaration><declarations-list>|<empty>", -1, -1, DECLARATIONS_LIST, -1);
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult item = getItem(index);

	// if begin it means that declaration-list is empty or ended
	if (item.getCode() == BEGIN) {
		tree.addNext("<empty>", -1, -1, EMPTY, -1);
		tree.switchTo(root);
		return item;
	}

	item = caseDeclaration(index);
	if (errorSyntaxHappened) return nullableResult;
	tree.switchTo(it);

	return caseDeclarationList(item.getIndexInResultVector(), root);
}

// 6. <declaration> --> <variable-identifier>:<attribute><attributes-list>;
LexerResult SyntaxAnalyzer::caseDeclaration(int index) {
	tree.addNext("<declaration> --><variable-identifier>:<attribute><attributes-list>;", -1, -1, DECLARATION, -1);
	Tree::TreeItem* it = tree.getCurrent();
	tree.addNext("<variable-identifier> --> <identifier>", -1, -1, VARIABLE_IDENTIFIER, -1);
	LexerResult nextItem = caseIdentifier(index);
	

	if (errorSyntaxHappened) return nullableResult;

	// nextItem has index + 1
	if (nextItem.getCode() != COLON) { // ':' code 0
		handleError("':' expected", nextItem);
	}
	tree.switchTo(it);
	tree.addChild(nextItem.getToken(), nextItem.getRowNumber(), nextItem.getColumnNumber(), ADDING_SEPARATED, COLON);
	if (errorSyntaxHappened) return nullableResult;

	// attribute for index + 2
	nextItem = caseAttribute(nextItem.getIndexInResultVector() + 1, it);
	if (errorSyntaxHappened) return nullableResult;

	nextItem = caseAttributeList(nextItem.getIndexInResultVector());
	if (errorSyntaxHappened) return nullableResult;
	tree.switchTo(it);
	return nextItem;
}

// 7. <attributes-list> --> <attribute> <attributes-list> | <empty>
LexerResult SyntaxAnalyzer::caseAttributeList(int index) {
	tree.addNext("<attributes-list> --> <attribute> <attributes-list> | <empty>", -1, -1, ATTRIBUTES_LIST, -1);
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult item = getItem(index);

	if (item.getCode() == SEMI_COLON) { // ';' code 1
		// attributr-list is empty or ended
		tree.addChild("<empty>", -1, -1, EMPTY, -1);
		return getItem(index + 1);
	}

	item = caseAttribute(index, it);
	tree.switchTo(it);
	return errorSyntaxHappened ? nullableResult : caseAttributeList(item.getIndexInResultVector());
}

// 8. < attribute > --> INTEGER | FLOAT | [<range>]
LexerResult SyntaxAnalyzer::caseAttribute(int index, Tree::TreeItem* root) {
	tree.addNext("< attribute > -- > INTEGER | FLOAT | [<range>]", -1, -1, ATTRIBUTE, -1);
	 LexerResult item = getItem(index);

 	 if (item.getCode() == INTEGER || item.getCode() == FLOAT) {
		 tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_RESERVED_WORD, item.getCode());
		 tree.switchTo(root);
		 return getItem(index + 1);
	 }

	 if (item.getCode() == LEFT_SQUARE_BRACKET) { // '[' code 2
		 tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), RANGE_SEPARATED_SYMBOL, item.getCode());
		 Tree::TreeItem* it = tree.getCurrent();
		 item = caseRange(index + 1);
		 if (errorSyntaxHappened) return nullableResult;
		 if (item.getCode() != RIGHT_SQUARE_BRACKET) { // ']' code 3
			 handleError("expected ]", item);
			 return nullableResult;
		 }
		 tree.switchTo(it);
		 tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), RANGE_SEPARATED_SYMBOL, item.getCode());
		 tree.switchTo(root);
		 return getItem(item.getIndexInResultVector() + 1);
	 }

	 handleError("expected  INTEGER | FLOAT | [<range>]", item);

	 return nullableResult;
 }

// 9. <range> --> <unsigned-integer> .. <unsigned-integer>
LexerResult SyntaxAnalyzer::caseRange(int index) {
	tree.addNext("<range> --> <unsigned-integer> .. <unsigned-integer>", -1, -1, RANGE, -1);
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult item = getItem(index);

	if (item.getCode() < 501 || item.getCode() >= 1001) {
		handleError("<unsigned - integer> expected", item);
		return nullableResult;
	}
	tree.addNext("<unsigned-integer>", -1, -1, UNSIGNED_INTEGER, -1);
	int code = item.getCode();
	tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_CONSTANT, code);
	tree.switchTo(it);
	item = getItem(index + 1);

	if (item.getCode() != DOUBLE_DOT) { // '..' code 302
		handleError(".. expected", item);
		return nullableResult;
	}
	tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), RANGE_SEPARATED_SYMBOL, DOUBLE_DOT);
	item = getItem(index + 2);

	if (item.getCode() < 501 || item.getCode() >= 1001) {
		handleError("<unsigned - integer> expected", item);
		return nullableResult;
	}
	tree.addNext("<unsigned-integer>",-1, -1, UNSIGNED_INTEGER, -1);
	tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_CONSTANT, item.getCode());
	tree.switchTo(it);
	return getItem(index + 3);
}

// 10. <statements-list> --> <statement> <statements-list> | <empty>
LexerResult SyntaxAnalyzer::caseStatementList(int index, int code) {
	tree.addNext("<statements-list> --> <statement> <statements-list> | <empty>", -1, -1, STATEMENT_LIST, -1);
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult item = getItem(index);

	// if met END or ENDLOOP means 
	// end
	if (item.getCode() == code) {
		tree.addNext("<empty>", -1, -1, EMPTY, -1);
		return item;
	}

	item = caseStatement(item.getIndexInResultVector());
	tree.switchTo(it);
	if (errorSyntaxHappened) return nullableResult;

	return caseStatementList(item.getIndexInResultVector(), code);
}

// 11. <statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP ;
LexerResult SyntaxAnalyzer::caseStatement(int index) {
	 tree.addNext("<statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP;", -1, -1, STATEMENT, -1);
	 LexerResult item = getItem(index);
	 Tree::TreeItem* current = tree.getCurrent();

	 if (item.getCode() >= 1001) {
		 // variable 
		 item = caseVariable(item.getIndexInResultVector());
		 if (item.getCode() != EQUALS) {
			 handleError(":= expected", item);
			 return nullableResult;
		 }
		 tree.switchTo(current);
		 tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_RESERVED_WORD, item.getCode());
		 item = caseExpression(item.getIndexInResultVector() + 1);

		 if (errorSyntaxHappened) return nullableResult;
		 // LOOP
	 } else if (item.getCode() == LOOP) { // LOOP - 406
			 
		 tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_RESERVED_WORD, item.getCode());
		 item = caseStatementList(item.getIndexInResultVector() + 1, ENDLOOP);
		 tree.switchTo(current);
		
		 if (errorSyntaxHappened) return nullableResult;

		 if (item.getCode() != ENDLOOP) { //ENDLOOP - 407
			 handleError("ENDLOOP expected", item);
			 return nullableResult;
		 }
		 tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_RESERVED_WORD, item.getCode());
		 item = getItem(item.getIndexInResultVector() + 1);
	 } else {
		 handleError("<variable> or LOOP expected", item);
		 return nullableResult;
	 }
	 tree.switchTo(current);
	 if (item.getCode() == SEMI_COLON) { // ';' code 1
		// attributr-list is empty or ended
		 tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_SEPARATED, item.getCode());
		 return getItem(item.getIndexInResultVector() + 1);
	 }
	
	 handleError("; expected", item);
	 return nullableResult;

 }

// 12. <expression> --> <variable> | <unsigned-integer>
LexerResult SyntaxAnalyzer::caseExpression(int index) {
	tree.addNext("<expression> --> <variable> | <unsigned-integer>", -1, -1, EXPRESSION, -1);
	LexerResult item = getItem(index);

	if (item.getCode() >= 501 && item.getCode() < 1001) {
		tree.addNext("<unsigned-integer>", -1, -1, UNSIGNED_INTEGER, -1);
		tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_CONSTANT, item.getCode());
		return getItem(index + 1);
	}

	item = caseVariable(item.getIndexInResultVector());

	if (errorSyntaxHappened) return nullableResult;

	return item;
 }

// 13. <variable> --> <variable-identifier><dimension>
LexerResult SyntaxAnalyzer::caseVariable(int index) {
	Tree::TreeItem* cur = tree.getCurrent();
	Tree::TreeItem* it = tree.addNext("<variable> --> <variable-identifier><dimension>", -1, -1, VARIABLE, -1);
	
	LexerResult item = caseVariableIdentifier(index);
	tree.switchTo(it);
	if (errorSyntaxHappened) return nullableResult;

	if (item.getCode() == LEFT_SQUARE_BRACKET) {
		item = caseDimension(item.getIndexInResultVector());
		if (errorSyntaxHappened) return nullableResult;
		return item;
	} else {
		tree.addNext("<dimension> --> [ <expression> ] | <empty>", -1, -1, DIMENSION, -1);
		tree.addChild("<empty>", -1, -1, EMPTY, -1);
	}
	tree.switchTo(cur);
	return item;
}

LexerResult SyntaxAnalyzer::caseVariableIdentifier(int index) {
	tree.addNext("<variable-identifier> --> <identifier>", -1, -1, VARIABLE_IDENTIFIER, -1);
	return caseIdentifier(index);
}

// 14. <dimension> --> [ <expression> ] | <empty>
LexerResult SyntaxAnalyzer::caseDimension(int index) {
	tree.addNext("<dimension> --> [ <expression> ] | <empty>", -1, -1, DIMENSION, -1);
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult item = getItem(index);

	if (item.getCode() != LEFT_SQUARE_BRACKET) {
		handleError("[ expected", item);
		return nullableResult;
	}

	tree.addChild(item.getToken(), -1, -1, RANGE_SEPARATED_SYMBOL, LEFT_SQUARE_BRACKET);
	item = caseExpression(item.getIndexInResultVector() + 1);
	tree.switchTo(it);

	if (item.getCode() != RIGHT_SQUARE_BRACKET) {
		handleError("] expected", item);
		return nullableResult;
	}

	tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), RANGE_SEPARATED_SYMBOL, RIGHT_SQUARE_BRACKET);
	return getItem(item.getIndexInResultVector() + 1);
}

// 16,16,17. <identifier> --> <letter><string>
LexerResult SyntaxAnalyzer::caseIdentifier(int index) {
	tree.addNext("<identifier> --> <letter><string>", -1, -1, IDENTIFIER_RULE, -1);
	LexerResult item = getItem(index);
	tree.addChild(item.getToken(), item.getRowNumber(), item.getColumnNumber(), ADDING_INDENTIFIER, item.getCode());

	if (item.getCode() >= 1001) {
		return getItem(index + 1);
	}

	handleError("<identifier> expected", item);

	return nullableResult;
}