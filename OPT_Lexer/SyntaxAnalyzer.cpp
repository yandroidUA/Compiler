#include "SyntaxAnalyzer.h"

LexerResult nullableResult("", -1, -1, -1, -1);
bool errorSyntaxHappened = false;

SyntaxAnalyzer::SyntaxAnalyzer(std::vector<LexerResult>& res) {
	this->lexerResults = res;
}

void SyntaxAnalyzer::analyze(){  
	tree.addChild("<signal-program> --> <program>");
	if (caseProgram(0)) {
		// tree.print();
	}
	tree.print();
}

LexerResult SyntaxAnalyzer::getItem(int index){
	if (lexerResults.size() <= index) {
		errorSyntaxHappened = true;
		std::cout << "Unexpected end of file" << std::endl;
		return nullableResult;
	}
	return lexerResults.at(index);
}

void SyntaxAnalyzer::handleError(const char* message, LexerResult failedItem) {
	errorSyntaxHappened = true;
	std::cout << message << ", row = " << failedItem.getRowNumber() << " column = " << failedItem.getColumnNumber() << ", but got " << failedItem.getToken() << std::endl;
}

// 2. < program > -- > PROGRAM <procedure - identifier>;
bool SyntaxAnalyzer::caseProgram(int index) {
	Tree::TreeItem* it = tree.addNext("<program> --> PROGRAM <procedure-identifier>;< block > .");
	
	if (getItem(index).getCode() != 401) {
		handleError("PROGRAM expected", getItem(index));
		return false;
	}

	tree.addNext("<procedure-identifier> --> <identifier>");
	LexerResult nextItem = caseIdentifier(index + 1);
	tree.switchTo(it);

	if (errorSyntaxHappened) return false;

	if (nextItem.getCode() != 1) {
		handleError("; expected", nextItem);
		return false;
	}
	tree.addNext("<block> --> <variable-declarations> BEGIN <statements-list> END");
	nextItem = caseBlock(nextItem.getIndexInResultVector() + 1);
	return !errorSyntaxHappened;
}

// 3. < block > -- > <variable - declarations> BEGIN <statements - list> END
LexerResult SyntaxAnalyzer::caseBlock(int index) {
	Tree::TreeItem* current = tree.getCurrent();
	LexerResult item = caseVariableDeclarations(index);

	tree.switchTo(current);
	if (errorSyntaxHappened) return nullableResult;

	if (item.getCode() != 402) {
		handleError("BEGIN expected", item);
		return nullableResult;
	}

	tree.addChild("402 - BEGIN");
	item = caseStatementList(item.getIndexInResultVector() + 1, 403);

	tree.switchTo(current);
	if (errorSyntaxHappened) return nullableResult;

	if (item.getCode() != 403) {
		handleError("END expected", item);
		return nullableResult;
	}

	tree.addChild("403 - END");
	return item;
}

// 4. <variable-declarations> --> VAR <declarations-list>|<empty>
LexerResult SyntaxAnalyzer::caseVariableDeclarations(int index) {
	Tree::TreeItem* it = tree.getCurrent();
	tree.addNext("<variable-declarations> --> VAR <declarations-list>|<empty>");
	LexerResult item = getItem(index);

	if (item.getCode() != 404) {
		handleError("VAR expected", getItem(index));
		return nullableResult;
	}
	tree.addChild(404);
	return caseDeclarationList(item.getIndexInResultVector() + 1, it);
}

// 5. <declarations-list> --> <declaration><declarations-list>|<empty>
LexerResult SyntaxAnalyzer::caseDeclarationList(int index, Tree::TreeItem* root) {
	tree.addNext("<declarations-list> --> <declaration><declarations-list>|<empty>");
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult item = getItem(index);

	// if begin it means that declaration-list is empty or ended
	if (item.getCode() == 402) {
		tree.addNext("<empty>");
		tree.switchTo(root);
		return item;
	}

	item = caseDeclaration(index);
	if (errorSyntaxHappened) return nullableResult;
	tree.switchTo(it);

	return caseDeclarationList(item.getIndexInResultVector(), root);
}

// 6. <declaration> --><variable-identifier>:<attribute><attributes-list>;
LexerResult SyntaxAnalyzer::caseDeclaration(int index) {
	tree.addNext("<declaration> --><variable-identifier>:<attribute><attributes-list>;");
	Tree::TreeItem* it = tree.getCurrent();
	tree.addNext("<variable-identifier> --> <identifier>");
	LexerResult nextItem = caseIdentifier(index);
	

	if (errorSyntaxHappened) return nullableResult;

	// nextItem has index + 1
	if (nextItem.getCode() != 0) { // ':' code 0
		handleError("':' expected", nextItem);
	}
	tree.switchTo(it);
	tree.addChild(0);
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
	tree.addNext("<attributes-list> --> <attribute> <attributes-list> | <empty>");
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult item = getItem(index);

	if (item.getCode() == 1) { // ';' code 1
		// attributr-list is empty or ended
		tree.addChild("<empty>");
		return getItem(index + 1);
	}

	return caseAttribute(index, it);
}

// 8. < attribute > -- > INTEGER | FLOAT | [<range>]
LexerResult SyntaxAnalyzer::caseAttribute(int index, Tree::TreeItem* root) {
	tree.addNext("< attribute > -- > INTEGER | FLOAT | [<range>]");
	 LexerResult item = getItem(index);

 	 if (item.getCode() == 408 || item.getCode() == 405) {
		 std::string str = std::to_string(item.getCode());
		 tree.addChild(str);
		 tree.switchTo(root);
		 return getItem(index + 1);
	 }

	 if (item.getCode() == 2) { // '[' code 2
		 tree.addChild(2);
		 item = caseRange(index + 1);
		 if (errorSyntaxHappened) return nullableResult;
		 if (item.getCode() != 3) { // ']' code 3
			 handleError("expected ]", item);
			 return nullableResult;
		 }
		 tree.addChild(3);
		 tree.switchTo(root);
		 return getItem(item.getIndexInResultVector() + 1);
	 }

	 handleError("expected  INTEGER | FLOAT | [<range>]", item);

	 return nullableResult;
 }

// 9. <range> --> <unsigned-integer> .. <unsigned-integer>
LexerResult SyntaxAnalyzer::caseRange(int index) {
	tree.addNext("<range> --> <unsigned-integer> .. <unsigned-integer>");
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult item = getItem(index);

	if (item.getCode() < 501 || item.getCode() >= 1001) {
		handleError("<unsigned - integer> expected", item);
		return nullableResult;
	}
	tree.addNext("<unsigned-integer>");
	tree.addChild(item.getCode());
	tree.switchTo(it);
	item = getItem(index + 1);

	if (item.getCode() != 302) { // '..' code 302
		handleError(".. expected", item);
		return nullableResult;
	}
	tree.addChild("302 - ..");
	item = getItem(index + 2);

	if (item.getCode() < 501 || item.getCode() >= 1001) {
		handleError("<unsigned - integer> expected", item);
		return nullableResult;
	}
	tree.addNext("<unsigned-integer>");
	tree.addChild(item.getCode());
	tree.switchTo(it);
	return getItem(index + 3);
}

// 10. <statements-list> --> <statement> <statements-list> | <empty>
LexerResult SyntaxAnalyzer::caseStatementList(int index, int code) {
	tree.addNext("<statements-list> --> <statement> <statements-list> | <empty>");
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult item = getItem(index);

	// if met END or ENDLOOP means 
	// end
	if (item.getCode() == code) {
		tree.addNext("<empty>");
		return item;
	}

	item = caseStatement(item.getIndexInResultVector());
	tree.switchTo(it);
	if (errorSyntaxHappened) return nullableResult;

	return caseStatementList(item.getIndexInResultVector(), code);
}

// 11. <statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP ;
LexerResult SyntaxAnalyzer::caseStatement(int index) {
	 tree.addNext("<statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP;");
	 LexerResult item = getItem(index);
	 Tree::TreeItem* current = tree.getCurrent();

	 if (item.getCode() >= 1001) {
		 // variable 
		 item = caseVariable(item.getIndexInResultVector());
		 if (item.getCode() != 301) {
			 handleError(":= expected", item);
			 return nullableResult;
		 }
		 tree.switchTo(current);
		 tree.addChild("301 - :=");
		 item = caseExpression(item.getIndexInResultVector() + 1);

		 if (errorSyntaxHappened) return nullableResult;

	 } else {
		 // LOOP
		 if (item.getCode() != 406) { // LOOP - 406
			 handleError("LOOP expected", item);
			 return nullableResult;
		 }
		 tree.addChild("406 - LOOP");
		 item = caseStatementList(item.getIndexInResultVector() + 1, 407);
		 tree.switchTo(current);
		
		 if (errorSyntaxHappened) return nullableResult;

		 if (item.getCode() != 407) { //ENDLOOP - 407
			 handleError("ENDLOOP expected", item);
			 return nullableResult;
		 }
		 tree.addChild("407 - ENDLOOP");
		 item = getItem(item.getIndexInResultVector() + 1);
	 }
	 tree.switchTo(current);
	 if (item.getCode() == 1) { // ';' code 1
		// attributr-list is empty or ended
		 tree.addChild("1 - ;");
		 return getItem(item.getIndexInResultVector() + 1);
	 }
	
	 handleError("; expected", item);
	 return nullableResult;

 }

// 12. <expression> --> <variable> | <unsigned-integer>
LexerResult SyntaxAnalyzer::caseExpression(int index) {
	tree.addNext("<expression> --> <variable> | <unsigned-integer>");
	LexerResult item = getItem(index);

	if (item.getCode() >= 501 && item.getCode() < 1001) {
		tree.addNext("<unsigned-integer>");
		std::string str = std::to_string(item.getCode());
		tree.addChild(str);
		return getItem(index + 1);
	}

	item = caseVariable(item.getIndexInResultVector());

	if (errorSyntaxHappened) return nullableResult;

	return item;
 }

// 13. <variable> --> <variable-identifier><dimension>
LexerResult SyntaxAnalyzer::caseVariable(int index) {
	 LexerResult item = caseIdentifier(index);

	 if (errorSyntaxHappened) return nullableResult;

	 if (item.getCode() == 2) {
		 item = caseDimension(item.getIndexInResultVector());
		 if (errorSyntaxHappened) return nullableResult;
		 return item;
	 }

	 return item;
 }

// 14. <dimension> --> [ <expression> ] | <empty>
LexerResult SyntaxAnalyzer::caseDimension(int index) {
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult item = getItem(index);

	if (item.getCode() != 2) {
		handleError("[ expected", item);
		return nullableResult;
	}

	tree.addChild(2);
	item = caseExpression(item.getIndexInResultVector() + 1);
	tree.switchTo(it);

	if (item.getCode() != 3) {
		handleError("] expected", item);
		return nullableResult;
	}

	tree.addChild(3);
	return getItem(item.getIndexInResultVector() + 1);
}

// 16,16,17. <identifier> --> <letter><string>
LexerResult SyntaxAnalyzer::caseIdentifier(int index) {
	tree.addNext("<identifier> --> <letter><string>");
	LexerResult item = getItem(index);
	std::string str = std::to_string(item.getCode());
	tree.addChild(str);

	if (item.getCode() >= 1001) {
		return getItem(index + 1);
	}

	handleError("<identifier> expected", item);

	return nullableResult;
}