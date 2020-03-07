#include "SyntaxAnalyzer.h"

LexerResult nullableResult("", -1, -1, -1, -1);
bool errorSyntaxHappened = false;

SyntaxAnalyzer::SyntaxAnalyzer(std::vector<LexerResult>& res) {
	this->lexerResults = res;
}

void SyntaxAnalyzer::analyze(){  
	caseProgram(0);
}

LexerResult SyntaxAnalyzer::getItem(int index){
	return lexerResults.at(index);
}

void SyntaxAnalyzer::handleError(const char* message, LexerResult failedItem) {
	errorSyntaxHappened = true;
	std::cout << message << ", row = " << failedItem.getRowNumber() << " column = " << failedItem.getColumnNumber() << ", but got " << failedItem.getToken() << std::endl;
}

bool SyntaxAnalyzer::caseProgram(int index) {

	if (getItem(index).getCode() != 401) {
		handleError("PROGRAM expected", getItem(index));
		return false;
	}


	LexerResult nextItem = caseIdentifier(index + 1);

	if (errorSyntaxHappened) return false;

	if (nextItem.getCode() != 1) {
		handleError("; expected", nextItem);
		return false;
	}

	nextItem = caseBlock(nextItem.getIndexInResultVector() + 1);

	return errorSyntaxHappened;
}

 LexerResult SyntaxAnalyzer::caseIdentifier(int index) {
	LexerResult item = getItem(index);

	if (item.getCode() >= 1001) {
		return getItem(index + 1);
	}

	handleError("<identifier> expected", item);

	return nullableResult;
}

// 3. < block > -- > <variable - declarations> BEGIN <statements - list> END
 LexerResult SyntaxAnalyzer::caseBlock(int index) {
	 LexerResult item = caseVariableDeclarations(index);

	 if (errorSyntaxHappened) return nullableResult;

	 if (item.getCode() != 402) {
		 handleError("BEGIN expected", item);
		 return nullableResult;
	 }
	
	 item = caseStatementList(item.getIndexInResultVector() + 1, 403);

	 if (errorSyntaxHappened) return nullableResult;

	 if (item.getCode() != 403) {
		 handleError("END expected", item);
		 return nullableResult;
	 }

	 return item;
 }

 LexerResult SyntaxAnalyzer::caseVariableDeclarations(int index) {
	 LexerResult item = getItem(index);

	 if (item.getCode() != 404) {
		 handleError("VAR expected", getItem(index));
		 return nullableResult;
	 }

	 return caseDeclarationList(item.getIndexInResultVector() + 1);
 }

 LexerResult SyntaxAnalyzer::caseDeclarationList(int index) {
	 LexerResult item = getItem(index);

	 // if begin it means that declaration-list is empty or ended
	 if (item.getCode() == 402) {
		 return item;
	 }

	 item = caseDeclaration(index);
	 if (errorSyntaxHappened) return nullableResult;

	 return caseDeclarationList(item.getIndexInResultVector());
 }

 LexerResult SyntaxAnalyzer::caseDeclaration(int index) {
	 //TODO: 6. <declaration> --><variable-identifier>:<attribute><attributes-list> ;
	 LexerResult nextItem = caseIdentifier(index);

	 if (errorSyntaxHappened) return nullableResult;

	 // nextItem has index + 1
	 if (nextItem.getCode() != 0) { // ':' code 0
		 handleError("':' expected", nextItem);
	 }

	 if (errorSyntaxHappened) return nullableResult;

	 // attribute for index + 2
	 nextItem = caseAttribute(nextItem.getIndexInResultVector() + 1);
	 if (errorSyntaxHappened) return nullableResult;

	 nextItem = caseAttributeList(nextItem.getIndexInResultVector());
	 if (errorSyntaxHappened) return nullableResult;

	 return nextItem;
 }

 LexerResult SyntaxAnalyzer::caseAttribute(int index) {
	 LexerResult item = getItem(index);

 	 if (item.getCode() == 408 || item.getCode() == 405) {
		 return getItem(index + 1);
	 }

	 if (item.getCode() == 2) { // '[' code 2
		 item = caseRange(index + 1);
		 if (errorSyntaxHappened) return nullableResult;
		 if (item.getCode() != 3) { // ']' code 3
			 handleError("expected ]", item);
			 return nullableResult;
		 }
		 return getItem(item.getIndexInResultVector() + 1);
	 }

	 handleError("expected  INTEGER | FLOAT | [<range>]", item);

	 return nullableResult;
 }

 LexerResult SyntaxAnalyzer::caseRange(int index) {
	 LexerResult item = getItem(index);

	 if (item.getCode() < 501 || item.getCode() >= 1001) {
		 handleError("<unsigned - integer> expected", item);
		 return nullableResult;
	 }

	 item = getItem(index + 1);

	 if (item.getCode() != 302) { // '..' code 302
		 handleError(".. expected", item);
		 return nullableResult;
	 }

	 item = getItem(index + 2);

	 if (item.getCode() < 501 || item.getCode() >= 1001) {
		 handleError("<unsigned - integer> expected", item);
		 return nullableResult;
	 }

	 return getItem(index + 3);
 }

 // 11. <statement> --> <variable> := <expression> ; | LOOP <statements-list> ENDLOOP ;
 LexerResult SyntaxAnalyzer::caseStatement(int index) {
	 LexerResult item = getItem(index);

	 if (item.getCode() >= 1001) {
		 // variable 
		 item = caseVariable(item.getIndexInResultVector());
		 if (item.getCode() != 301) {
			 handleError(":= expected", item);
			 return nullableResult;
		 }

		 item = caseExpression(item.getIndexInResultVector() + 1);

		 if (errorSyntaxHappened) return nullableResult;

	 } else {
		 // LOOP
		 if (item.getCode() != 406) { // LOOP - 406
			 handleError("LOOP expected", item);
			 return nullableResult;
		 }

		 item = caseStatementList(item.getIndexInResultVector() + 1, 407);

		 if (errorSyntaxHappened) return nullableResult;

		 if (item.getCode() != 407) { //ENDLOOP - 407
			 handleError("ENDLOOP expected", item);
			 return nullableResult;
		 }
		 item = getItem(item.getIndexInResultVector() + 1);
	 }

	 if (item.getCode() == 1) { // ';' code 1
		// attributr-list is empty or ended
		 return getItem(item.getIndexInResultVector() + 1);
	 }

	 handleError("; expected", item);
	 return nullableResult;

 }

 // 12. <expression> --> <variable> | <unsigned-integer>
 LexerResult SyntaxAnalyzer::caseExpression(int index) {
	LexerResult item = getItem(index);

	if (item.getCode() >= 501 && item.getCode() < 1001) {
		return getItem(index + 1);
	}

	item = caseVariable(item.getIndexInResultVector());

	if (errorSyntaxHappened) return nullableResult;

	return item;
 }

 LexerResult SyntaxAnalyzer::caseAttributeList(int index) {
	 LexerResult item = getItem(index);

	 if (item.getCode() == 1) { // ';' code 1
		 // attributr-list is empty or ended
		 return getItem(index + 1);
	 }

	 return caseAttribute(index);
 }

 // 10. <statements-list> --> <statement> <statements-list> | <empty>
 LexerResult SyntaxAnalyzer::caseStatementList(int index, int code) {
	 LexerResult item = getItem(index);

	 // if met END or ENDLOOP means 
	 // end
	 if (item.getCode() == code) {
		 return item;
	 }

	 item = caseStatement(item.getIndexInResultVector());

	 if (errorSyntaxHappened) return nullableResult;

	 return caseStatementList(item.getIndexInResultVector(), code);
 }

 // 14. <dimension> --> [ <expression> ] | <empty>
 LexerResult SyntaxAnalyzer::caseDimension(int index) {
	 LexerResult item = getItem(index);

	 if (item.getCode() != 2) {
		 handleError("[ expected", item);
		 return nullableResult;
	 }

	 item = caseExpression(item.getIndexInResultVector() + 1);

	 if (item.getCode() != 3) {
		 handleError("] expected", item);
		 return nullableResult;
	 }

	 return getItem(item.getIndexInResultVector() + 1);
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
