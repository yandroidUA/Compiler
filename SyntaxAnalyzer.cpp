#include "SyntaxAnalyzer.h"
#include <fstream>
#include "Rules.h"
#include "ReservedWords.h"

SyntaxAnalyzer::SyntaxAnalyzer(std::vector<LexerResult>& res) {
	this->lexerResults = res;
	this->errorMessageSyntax = "";
	this->errorSyntaxHappened = false;
}

void SyntaxAnalyzer::analyze(){  
	tree.addChild("<signal-program> --> <program>", -1, -1, SIGNAL_PROGRAM, -1);
	caseProgram();
		// tree.print();
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

LexerResult* SyntaxAnalyzer::getItem(int index){
	if (lexerResults.size() <= index) {
		errorSyntaxHappened = true;
		std::cout << "Unexpected end of file" << std::endl;
		errorMessageSyntax = "Unexpected end of file";
		return nullptr;
	}
	return &(lexerResults.at(index));
}

void SyntaxAnalyzer::handleError(const char* message, LexerResult* failedItem) {
	errorSyntaxHappened = true;
	std::cout << message << ", row = " << failedItem->getRowNumber() << " column = " << failedItem->getColumnNumber() << ", but got " << failedItem->getToken() << std::endl;
	errorMessageSyntax = message;
	errorMessageSyntax.append(", row = ");
	errorMessageSyntax.append(std::to_string(failedItem->getRowNumber()));
	errorMessageSyntax.append(" column = ");
	errorMessageSyntax.append(std::to_string(failedItem->getColumnNumber()));
	errorMessageSyntax.append(" but got ");
	errorMessageSyntax.append(failedItem->getToken());
}

// 2. < program > --> PROGRAM <procedure - identifier>;
bool SyntaxAnalyzer::caseProgram() {
	Tree::TreeItem* it = tree.addNext("<program> --> PROGRAM <procedure-identifier>;< block > .", -1, -1, PROGRAM_RULE, -1);
	
	LexerResult* item = getItem(0);

	if (item->getCode() != PROGRAM) {
		handleError("PROGRAM expected", item);
		return false;
	}

	tree.addNext("<procedure-identifier> --> <identifier>", -1, -1, PROCEDURE_IDENTIFIER, -1);
	item = getItem(item->getIndexInResultVector() + 1);

	bool isIdentifier = caseIdentifier(item);
	if (!isIdentifier) {
		handleError("<identifier> expected", item);
		return false;
	}

	item = getItem(item->getIndexInResultVector() + 1);
	tree.switchTo(it);

	if (item->getCode() != SEMI_COLON) {
		handleError("; expected", item);
		return false;
	}

	tree.addNext("<block> --> <variable-declarations> BEGIN <statements-list> END", -1, -1, BLOCK, -1);
	return caseBlock(item->getIndexInResultVector() + 1) != nullptr;
}

// 3. < block > --> <variable - declarations> BEGIN <statements - list> END
LexerResult* SyntaxAnalyzer::caseBlock(int index) {
	Tree::TreeItem* current = tree.getCurrent();
	
	LexerResult* item = caseVariableDeclarations(index);
	if (item == nullptr) return nullptr;

	tree.switchTo(current);

	if (item->getCode() != BEGIN) {
		handleError("BEGIN expected", item);
		return nullptr;
	}

	tree.addChild(item, ADDING_RESERVED_WORD);
	item = caseStatementList(item->getIndexInResultVector() + 1);
	if (item == nullptr) return nullptr;

	tree.switchTo(current);

	if (item->getCode() != END) {
		handleError("END expected", item);
		return nullptr;
	}

	tree.addChild(item, ADDING_RESERVED_WORD);
	return getItem(item->getIndexInResultVector() + 1);
}

// 4. <variable-declarations> --> VAR <declarations-list>|<empty>
LexerResult* SyntaxAnalyzer::caseVariableDeclarations(int index) {
	Tree::TreeItem* it = tree.getCurrent();

	tree.addNext("<variable-declarations> --> VAR <declarations-list>|<empty>", -1, -1, VARIABLE_DECLARATIONS, -1);
	LexerResult* item = getItem(index);

	if (item->getCode() != VAR) {
		tree.addChild("<empty>", -1, -1, EMPTY, -1);
		return item;
	}

	tree.addChild(item, ADDING_RESERVED_WORD);
	return caseDeclarationList(item->getIndexInResultVector() + 1);
}

// 5. <declarations-list> --> <declaration><declarations-list>|<empty>
LexerResult* SyntaxAnalyzer::caseDeclarationList(int index) {
	tree.addNext("<declarations-list> --> <declaration><declarations-list>|<empty>", -1, -1, DECLARATIONS_LIST, -1);
	Tree::TreeItem* it = tree.getCurrent();

	std::pair<LexerResult*, bool> declarationParsingResult = caseDeclaration(index);
	if (declarationParsingResult.second) {
		tree.switchTo(it);
		tree.addChild("<empty>", -1, -1, EMPTY, -1);
		return declarationParsingResult.first;
	}

	if (declarationParsingResult.first == nullptr) return nullptr;
	tree.switchTo(it);

	return caseDeclarationList(declarationParsingResult.first->getIndexInResultVector() + 1);
}

// 6. <declaration> --> <variable-identifier>:<attribute><attributes-list>;
std::pair<LexerResult*, bool> SyntaxAnalyzer::caseDeclaration(int index) {
	Tree::TreeItem* it = nullptr;
	LexerResult* item = getItem(index);


	bool isIdentifier = caseVariableIdentifier(item, [&]() {
		it = tree.addNext("<declaration> --><variable-identifier>:<attribute><attributes-list>;", -1, -1, DECLARATION, -1);
	});

	if (!isIdentifier) {
		if (item->getCode() == COLON) {
			handleError("<identifier> expected", item);
			return std::make_pair(item, false);
		} else {
			return std::make_pair(item, true);
		}
	}

	item = getItem(item->getIndexInResultVector() + 1);
	if (item->getCode() != COLON) {
		handleError("':' expected", item);
		return std::make_pair(item, false);
	}

	tree.switchTo(it);
	tree.addChild(item, ADDING_SEPARATED);

	std::pair<LexerResult*, bool> attributeParsingResult = caseAttribute(item->getIndexInResultVector() + 1);
	item = attributeParsingResult.first;
	if (item == nullptr) return std::make_pair(item, false);

	if (item->getCode() != SEMI_COLON && attributeParsingResult.second) {
		handleError("expected  INTEGER | FLOAT | [<range>]", item);
		return std::make_pair(nullptr, false);
	}

	if (attributeParsingResult.second) return attributeParsingResult;

	tree.switchTo(it);
	item = caseAttributeList(item->getIndexInResultVector());
	tree.switchTo(it);
	
	if (item == nullptr) return std::make_pair(nullptr, false);

	if (item->getCode() != SEMI_COLON) {
		handleError("Expected ';'", item);
		return std::make_pair(nullptr, false);
	}

	return std::make_pair(item, false);
}

// 7. <attributes-list> --> <attribute> <attributes-list>; | <empty>
LexerResult* SyntaxAnalyzer::caseAttributeList(int index) {
	tree.addNext("<attributes-list> --> <attribute> <attributes-list> | <empty>", -1, -1, ATTRIBUTES_LIST, -1);
	Tree::TreeItem* it = tree.getCurrent();

	std::pair<LexerResult*, bool> attributeParseResult = caseAttribute(index);

	if (attributeParseResult.first->getCode() != SEMI_COLON && attributeParseResult.second) {
		handleError("';' expected", attributeParseResult.first);
		return nullptr;
	}

	if (attributeParseResult.second) {
		tree.switchTo(it);
		tree.addChild("<empty>", -1, -1, EMPTY, -1);
		return getItem(attributeParseResult.first->getIndexInResultVector());
	}

	tree.switchTo(it);
	return attributeParseResult.first != nullptr ? caseAttributeList(attributeParseResult.first->getIndexInResultVector()) : nullptr;
}

// 8. < attribute > --> INTEGER | FLOAT | [<range>]
std::pair<LexerResult*, bool> SyntaxAnalyzer::caseAttribute(int index) {
	LexerResult* item = getItem(index);

	if (item->getCode() == INTEGER || item->getCode() == FLOAT) {
		tree.addNext("< attribute > -- > INTEGER | FLOAT | [<range>]", -1, -1, ATTRIBUTE, -1);
		tree.addChild(item, ADDING_RESERVED_WORD);
		return std::make_pair(getItem(item->getIndexInResultVector() + 1), false);
	}

	if (item->getCode() == LEFT_SQUARE_BRACKET) {
		tree.addNext("< attribute > -- > INTEGER | FLOAT | [<range>]", -1, -1, ATTRIBUTE, -1);
		tree.addChild(item, RANGE_SEPARATED_SYMBOL);
		Tree::TreeItem* it = tree.getCurrent();

		item = caseRange(item->getIndexInResultVector() + 1);
		if (item == nullptr) return std::make_pair(nullptr, false);
		item = getItem(item->getIndexInResultVector());

		if (item->getCode() != RIGHT_SQUARE_BRACKET) {
			handleError("] expected", item);
			return std::make_pair(nullptr, false);
		}

		tree.switchTo(it);
		tree.addChild(item, RANGE_SEPARATED_SYMBOL);

		return std::make_pair(getItem(item->getIndexInResultVector() + 1), false);
	}

	return std::make_pair(item, true);
}

// 9. <range> --> <unsigned-integer> .. <unsigned-integer>
LexerResult* SyntaxAnalyzer::caseRange(int index) {
	tree.addNext("<range> --> <unsigned-integer> .. <unsigned-integer>", -1, -1, RANGE, -1);
	Tree::TreeItem* it = tree.getCurrent();

	LexerResult* item = getItem(index);

	if (item->getCode() < 501 || item->getCode() >= 1001) {
		handleError("<unsigned - integer> expected", item);
		return nullptr;
	}

	tree.addNext("<unsigned-integer>", -1, -1, UNSIGNED_INTEGER, -1);
	tree.addChild(item, ADDING_CONSTANT);
	tree.switchTo(it);

	item = getItem(item->getIndexInResultVector() + 1);
	if (item->getCode() != DOUBLE_DOT) {
		handleError(".. expected", item);
		return nullptr;
	}

	tree.addChild(item, RANGE_SEPARATED_SYMBOL);
	item = getItem(item->getIndexInResultVector() + 1);

	if (item->getCode() < 501 || item->getCode() >= 1001) {
		handleError("<unsigned - integer> expected", item);
		return nullptr;
	}

	tree.addNext("<unsigned-integer>",-1, -1, UNSIGNED_INTEGER, -1);
	tree.addChild(item, ADDING_CONSTANT);
	tree.switchTo(it);

	return getItem(item->getIndexInResultVector() + 1);
}

// 10. <statements-list> --> <statement> <statements-list> | <empty>
LexerResult* SyntaxAnalyzer::caseStatementList(int index) {
	tree.addNext("<statements-list> --> <statement> <statements-list> | <empty>", -1, -1, STATEMENT_LIST, -1);
	Tree::TreeItem* it = tree.getCurrent();

	std::pair<LexerResult*, bool> statementParseResult = caseStatement(index);
	tree.switchTo(it);
	if (statementParseResult.second) {
		tree.addChild("<empty>", -1, -1, EMPTY, -1);
		return statementParseResult.first;
	}

	if (statementParseResult.first == nullptr) return nullptr;
	return caseStatementList(statementParseResult.first->getIndexInResultVector());
}

// 11. <statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP ;
std::pair<LexerResult*, bool> SyntaxAnalyzer::caseStatement(int index) {
	 LexerResult* item = getItem(index);
	 Tree::TreeItem* current = nullptr;

	 if (item->getCode() >= 1001) {
		 tree.addNext("<statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP;", -1, -1, STATEMENT, -1);
		 current = tree.getCurrent();

		 item = caseVariable(item->getIndexInResultVector());
		 if (item == nullptr) return std::make_pair(nullptr, false);

		 if (item->getCode() != EQUALS) {
			 handleError(":= expected", item);
			 return std::make_pair(nullptr, false);
		 }

		 tree.addChild(item, ADDING_RESERVED_WORD);
		 item = caseExpression(item->getIndexInResultVector() + 1);

		 if (item == nullptr) return std::make_pair(item, false);
		 // LOOP
	 } else if (item->getCode() == LOOP) {
		tree.addNext("<statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP;", -1, -1, STATEMENT, -1);
		 current = tree.getCurrent();

		 tree.addChild(item, ADDING_RESERVED_WORD);
		 item = caseStatementList(item->getIndexInResultVector() + 1);
		 if (item == nullptr) return std::make_pair(nullptr, false);
		 tree.switchTo(current);

		 if (item->getCode() != ENDLOOP) {
			 handleError("ENDLOOP expected", item);
			 return std::make_pair(nullptr, false);
		 }

		 tree.addChild(item, ADDING_RESERVED_WORD);
		 item = getItem(item->getIndexInResultVector() + 1);

	 } else {
		 // handleError("<variable> or LOOP expected", item);
		 return std::make_pair(item, true);
	 }

	 tree.switchTo(current);

	 if (item->getCode() == SEMI_COLON) {
		// attributr-list is empty or ended
		 tree.addChild(item, ADDING_SEPARATED);
		 return std::make_pair(getItem(item->getIndexInResultVector() + 1), false);
	 }
	
	 handleError("; expected", item);
	 return std::make_pair(nullptr, false);
 }

// 12. <expression> --> <variable> | <unsigned-integer>
LexerResult* SyntaxAnalyzer::caseExpression(int index) {
	tree.addNext("<expression> --> <variable> | <unsigned-integer>", -1, -1, EXPRESSION, -1);
	LexerResult* item = getItem(index);

	if (item->getCode() >= 501 && item->getCode() < 1001) {
		tree.addNext("<unsigned-integer>", -1, -1, UNSIGNED_INTEGER, -1);
		tree.addChild(item, ADDING_CONSTANT);
		return getItem(item->getIndexInResultVector() + 1);
	}

	return caseVariable(item->getIndexInResultVector());
 }

// 13. <variable> --> <variable-identifier><dimension>
LexerResult* SyntaxAnalyzer::caseVariable(int index) {
	Tree::TreeItem* cur = tree.getCurrent();
	Tree::TreeItem* it = tree.addNext("<variable> --> <variable-identifier><dimension>", -1, -1, VARIABLE, -1);
	LexerResult* item = getItem(index);

	bool isVariableIdentifierParsed = caseVariableIdentifier(item);
	if (!isVariableIdentifierParsed) {
		handleError("<identifier> expected", item);
		return nullptr;
	}

	tree.switchTo(it);
	item = caseDimension(item->getIndexInResultVector() + 1);
	tree.switchTo(cur);
	return item;
}

bool SyntaxAnalyzer::caseVariableIdentifier(LexerResult* item, std::function<void()> callback) {
	return caseIdentifier(item, [&](LexerResult* item) {
		if (callback != nullptr) {
			callback();
		}
		tree.addNext("<variable-identifier> --> <identifier>", -1, -1, VARIABLE_IDENTIFIER, -1);
		tree.addNext("<identifier> --> <letter><string>", -1, -1, IDENTIFIER_RULE, -1);
		tree.addChild(item, ADDING_INDENTIFIER);
	});
}

// 14. <dimension> --> [ <expression> ] | <empty>
LexerResult* SyntaxAnalyzer::caseDimension(int index) {
	tree.addNext("<dimension> --> [ <expression> ] | <empty>", -1, -1, DIMENSION, -1);
	Tree::TreeItem* it = tree.getCurrent();
	LexerResult* item = getItem(index);

	if (item->getCode() != LEFT_SQUARE_BRACKET) {
		tree.addChild("<empty>", -1, -1, EMPTY, -1);
		return item;
	}

	tree.addChild(item, RANGE_SEPARATED_SYMBOL);
	item = caseExpression(item->getIndexInResultVector() + 1);

	if (item == nullptr) return nullptr;
	tree.switchTo(it);

	if (item->getCode() != RIGHT_SQUARE_BRACKET) {
		handleError("] expected", item);
		return nullptr;
	}

	tree.addChild(item, RANGE_SEPARATED_SYMBOL);
	return getItem(item->getIndexInResultVector() + 1);
}

// 16,16,17. <identifier> --> <letter><string>
bool SyntaxAnalyzer::caseIdentifier(LexerResult* item, std::function<void(LexerResult*)> callback) {
	if (item->getCode() >= 1001) {
		if (callback == nullptr) {
			tree.addNext("<identifier> --> <letter><string>", -1, -1, IDENTIFIER_RULE, -1);
			tree.addChild(item, ADDING_INDENTIFIER);
		} else {
			callback(item);
		}
		return true;
	}

	return false;
}