#include "Lexer.h"
#include <string>
#include <iostream>
#include "ReservedWords.h"

typedef std::pair<std::string, int> MapPair;

int TOKEN_STATUS_CONSTANT = 0;
int TOKEN_STATUS_IDENTIFIER = 1;
int TOKEN_STATUS_RESERVED_WORD = 2;
int TOKEN_STATUS_ONE_SEPARATED_TOKEN = 3;
int TOKEN_STATUS_MULTI_SEPARATED_TOKEN = 4;
int TOKEN_STATUS_COMMENT = 5;

int oneSymbolTokenIndex = 0;
int multiSymbolTokenIndex = 301;
int reservedWordsIndex = 401;
int constantsIndex = 501;
int variablesIndex = 1001;

bool Lexer::isLetter(int character) {
	for (auto& c : lettersVector) {
		if (c == character) {
			return true;
		}
	}
	return false;
}

bool Lexer::isNumber(int character) {
	for (auto& number : numbersVector) {
		if ((char) number == character) {
			return true;
		}
	}
	return false;
}

bool Lexer::isWhiteSpace(int character) {
	for (auto& whiteSpaceId : whiteSpacesVector) {
		if (whiteSpaceId == character) {
			return true;
		}
	}
	return false;
}

bool Lexer::isOneSeparated(int c) {
	for (auto& oneSeparatedToken : oneSeparatedTokens) {
		if (oneSeparatedToken == c) {
			return true;
		}
	}
	return false;
}

bool Lexer::isMultiSeparated(int c) {
	for (auto& multiSeparatedToken : multiSeparatedTokens) {
		if (multiSeparatedToken.getCode() == c) {
			return true;
		}
	}
	return false;
}

bool Lexer::isSecondPartOfMultiSeparated(int sepToken, int c) {
	for (auto& multiSeparatedToken : multiSeparatedTokens) {
		if (multiSeparatedToken.getCode() == sepToken && multiSeparatedToken.getPairCode() == c) {
			return true;
		}
	}
	return false;
}

void Lexer::resetTokenStatus() {
	currentTokenState[0] = 0;
	currentTokenState[1] = 0;
	currentTokenState[2] = 0;
	currentTokenState[3] = 0;
	currentTokenState[4] = 0;
	currentTokenState[5] = 0;
}

Lexer::Lexer() {
	addReservedWord("PROGRAM", PROGRAM);				//401
	addReservedWord("BEGIN", BEGIN);					//402
	addReservedWord("END", END);						//403
	addReservedWord("VAR", VAR);						//404
	addReservedWord("FLOAT", FLOAT);					//405
	addReservedWord("LOOP", LOOP);						//406
	addReservedWord("ENDLOOP", ENDLOOP);				//407
	addReservedWord("INTEGER", INTEGER);				//408
	addReservedWord("ELSE", ELSE);				//408
	addReservedWord("IF", IF);				//408
	addReservedWord("THEN", THEN);				//408
	addReservedWord("ENDIF", ENDIF);				//408


	addOneSeparatedToken(":", COLON);					//0
	addOneSeparatedToken(";", SEMI_COLON);				//1
	addOneSeparatedToken("[", LEFT_SQUARE_BRACKET);		//2
	addOneSeparatedToken("]", RIGHT_SQUARE_BRACKET);	//3
	addOneSeparatedToken(".", DOT);	//4
	
	addMultiSeparatedToken(":=", EQUALS);				//301
	addMultiSeparatedToken("..", DOUBLE_DOT);			//302
}

void Lexer::addToken(std::string&) {
	if (currentTokenState[TOKEN_STATUS_CONSTANT] == 1) {
		// std::cout << token << " is constant, row=" << savedRow << " column=" << savedColumn << std::endl;
		addTokenToResultVector(token, CONSTANT, savedColumn, savedRow);
		return;
	}

	if (currentTokenState[TOKEN_STATUS_IDENTIFIER] == 1 && currentTokenState[TOKEN_STATUS_RESERVED_WORD] == 0) {
		// std::cout << token << " is identifier, row=" << savedRow << " column=" << savedColumn << std::endl;
		addTokenToResultVector(token, IDENTIFIER, savedColumn, savedRow);
		return;
	}

	if (currentTokenState[TOKEN_STATUS_RESERVED_WORD] == 1 && currentTokenState[TOKEN_STATUS_IDENTIFIER] == 0) {
		// std::cout << token << " is reserved word, row=" << savedRow << " column=" << savedColumn << std::endl;
		addTokenToResultVector(token, RESERVED_WORD, savedColumn, savedRow);
		return;
	}

	if (currentTokenState[TOKEN_STATUS_ONE_SEPARATED_TOKEN] == 1) {
		// std::cout << token << " is one separated word, row=" << savedRow << " column=" << savedColumn << std::endl;
		addTokenToResultVector(token, ONE_SEPARATED_TOKEN, savedColumn, savedRow);
		return;
	}

	if (currentTokenState[TOKEN_STATUS_MULTI_SEPARATED_TOKEN] == 1) {
		// std::cout << token << " is multi separated word, row=" << savedRow << " column=" << savedColumn << std::endl;
		addTokenToResultVector(token, MULTI_SEPARATED_TOKEN, savedColumn, savedRow);
		return;
	}

	if (currentTokenState[TOKEN_STATUS_COMMENT] == 1) {
		// std::cout << token << " is comment, row=" << savedRow << " column=" << savedColumn << std::endl;
		return;
	}

	int reservedWordCode = isTokenReservedWord(token);
	if (reservedWordCode != -1) {
		// std::cout << token << " is reserved word, row=" << savedRow << "column=" << savedColumn << std::endl;
		addTokenToResultVector(token, RESERVED_WORD, savedColumn, savedRow);
		return;
	} else {
		// std::cout << token << " is identifier, row=" << savedRow << "column=" << savedColumn << std::endl;
		addTokenToResultVector(token, IDENTIFIER, savedColumn, savedRow);
		return;
	}

}

int Lexer::addIdentifier(std::string& token) {
	identifiersTokensMap.insert(MapPair(token, variablesIndex));
	int code = variablesIndex;
	variablesIndex++;
	return code;
}

int Lexer::addConstant(std::string& word) {
	constantTokensMap.insert(MapPair(word, constantsIndex));
	int code = constantsIndex;
	constantsIndex++;
	return code;
}

void Lexer::addTokenToResultVector(std::string& token, TokenStatus status, int column, int row) {
	int code = -1;
	switch (status) {
	case IDENTIFIER:
		code = isTokenIdentifier(token);
		if (code == -1) {
			code = addIdentifier(token);
		}
		lexerResultValues.push_back(LexerResult(token, code, row, column, lexerResultValues.size()));
		break;
	case RESERVED_WORD:
		code = isTokenReservedWord(token);
		lexerResultValues.push_back(LexerResult(token, code, row, column, lexerResultValues.size()));
		break;
	case ONE_SEPARATED_TOKEN:
		code = isTokenOneSeparatedToken(token);
		lexerResultValues.push_back(LexerResult(token, code, row, column, lexerResultValues.size()));
		break;
	case MULTI_SEPARATED_TOKEN:
		code = isTokenMultiSeparatedToken(token);
		lexerResultValues.push_back(LexerResult(token, code, row, column, lexerResultValues.size()));
		break;
	case CONSTANT:
		code = isTokenConstant(token);
		if (code == -1) {
			code = addConstant(token);
		}
		lexerResultValues.push_back(LexerResult(token, code, row, column, lexerResultValues.size()));
		break;
	}
}

void Lexer::addReservedWord(const char* word, int code) {
	reservedWords.insert(MapPair(word, code));
}

void Lexer::addOneSeparatedToken(const char* word, int code) {
	oneSeparatedTokensMap.insert(MapPair(word, code));
}

void Lexer::addMultiSeparatedToken(const char* word, int code) {
	multiSeparatedTokensMap.insert(MapPair(word, code));
}

int Lexer::isTokenReservedWord(std::string& word){
	for (auto& resWord : reservedWords) {
		if (resWord.first == word) {
			return resWord.second;
		}
	}
	return -1;
}

int Lexer::isTokenOneSeparatedToken(std::string& word) {
	for (auto& oneSeparetadToken : oneSeparatedTokensMap) {
		if (oneSeparetadToken.first == word) {
			return oneSeparetadToken.second;
		}
	}
	return -1;
}

int Lexer::isTokenMultiSeparatedToken(std::string& word) {
	for (auto& multiSeparatedToken : multiSeparatedTokensMap) {
		if (multiSeparatedToken.first == word) {
			return multiSeparatedToken.second;
		}
	}
	return -1;
}

int Lexer::isTokenIdentifier(std::string& word) {
	for (auto& indentifier : identifiersTokensMap) {
		if (indentifier.first == word) {
			return indentifier.second;
		}
	}
	return -1;
}

int Lexer::isTokenConstant(std::string& word) {
	for (auto& constant : constantTokensMap) {
		if (constant.first == word) {
			return constant.second;
		}
	}
	return -1;
}

int Lexer::readCharacterFromFile(std::ifstream &file) {
	char currentCaracter = file.eof();
	if (!file.eof()) {
		file.get(currentCaracter);
		currentColumn++;
	}

	return (int)currentCaracter;
}

int Lexer::caseMultiSeparated(int letter, std::ifstream& file) {
	token += (char)letter;
	
	while (!file.eof()) {
		int nextLetter = readCharacterFromFile(file);
		if (isSecondPartOfMultiSeparated(letter, nextLetter)) {
			currentTokenState[TOKEN_STATUS_MULTI_SEPARATED_TOKEN] = 1;
			currentTokenState[TOKEN_STATUS_ONE_SEPARATED_TOKEN] = 0;
			token += (char)nextLetter;
		} else {
			if (!token.empty()) {
				addToken(token);
			}
			token = "";
			resetTokenStatus();
			return nextLetter;
		}
	}
}

int Lexer::caseOneSeparated(int letter, std::ifstream& file) {
	resetTokenStatus();
	currentTokenState[TOKEN_STATUS_ONE_SEPARATED_TOKEN] = 1;
	while (!file.eof()) {
		if (isMultiSeparated(letter)) {
			return caseMultiSeparated(letter, file);
		} else {
			token = (char)letter;
			addToken(token);
			token = "";
			resetTokenStatus();
			return readCharacterFromFile(file);
		}
	}
}

int Lexer::caseLetter(int letter, std::ifstream& file) {
	token += (char)letter;
	currentTokenState[TOKEN_STATUS_IDENTIFIER] = 1;
	currentTokenState[TOKEN_STATUS_RESERVED_WORD] = 1;

	while (!file.eof()) {
		int nextLetter = readCharacterFromFile(file);

		if (isNumber(nextLetter)) {
			currentTokenState[TOKEN_STATUS_IDENTIFIER] = 1;
			token += (char)nextLetter;
			continue;
		}

		if (!isLetter(nextLetter)) {
			if (!token.empty()) {
				addToken(token);
			}
			token = "";
			resetTokenStatus();
			return nextLetter;
		}

		token += (char)nextLetter;
	}
}

int Lexer::caseNumber(int letter, std::ifstream& file) {
	currentTokenState[TOKEN_STATUS_CONSTANT] = 1;
	token += (char)letter;

	while (!file.eof()) {
		int nextLetter = readCharacterFromFile(file);
		if (!isNumber(nextLetter)) {

			if (!isOneSeparated(nextLetter) && !isWhiteSpace(nextLetter)) {
				handleError("Expected number or whitespace or separator, but got ", nextLetter, currentRow, currentColumn);
				return nextLetter;
			}

			if (!token.empty()) {
				addToken(token);
			}
			token = "";
			resetTokenStatus();
			return nextLetter;
		}

		token += (char)nextLetter;
	}
}

int Lexer::caseComment(int letter, std::ifstream& file) {
	resetTokenStatus();
	currentTokenState[TOKEN_STATUS_COMMENT] = 1;
	token += (char)letter;
	bool isStartFound = false;

	int nextLetter = readCharacterFromFile(file);

	if (nextLetter == '\0') {
		handleError("Unclosed comment", nextLetter, currentRow, currentColumn);
		return nextLetter;
	}

	if (nextLetter != '*') {
		handleError("Comment must start with '(*'", nextLetter, currentRow, currentColumn);
		return nextLetter;
	} else {
		token += (char)nextLetter;
	}

	while (!file.eof()) {
		nextLetter = readCharacterFromFile(file);

		if (nextLetter == '\0') {
			handleError("Unclosed comment", nextLetter, currentRow, currentColumn);
			return nextLetter;
		}

		if (nextLetter == '*') {
			isStartFound = true;
		} else if (nextLetter == ')' && isStartFound) {
			token += (char)nextLetter;
			addToken(token);
			token = "";
			resetTokenStatus();
			return readCharacterFromFile(file);
		} else {
			isStartFound = false;
		}

		if (nextLetter == '\n') { currentRow++; currentColumn = 0; }

		token += (char)nextLetter;

	}

}

void Lexer::handleError(const char* text, char letter, int row, int column) {
	errorHappened = true;
	std::cout << text << " character " << letter << " code: " << (int)letter << " (row: " << row << " column: " << column << ")" << std::endl;
	errorToken = text;
	errorToken.append(" character: ");
	errorToken.append(std::to_string(letter));
	errorToken.append(" code: ");
	errorToken.append(std::to_string((int)letter));
	errorToken.append(" (row: ");
	errorToken.append(std::to_string(row));
	errorToken.append(" column: ");
	errorToken.append(std::to_string(column));
	errorToken.append(")");
}

bool Lexer::isComment(int letter) {
	return letter == '(';
}

Lexer::AnalyzeResult Lexer::scanFile(std::string& filePath) {
	std::ifstream file;

	file.open(filePath, std::ios::in);

	int savedLetter = -1;

	while (!file.eof()) {
		int letter = savedLetter == -1 ? readCharacterFromFile(file) : savedLetter;

		if (isLetter(letter)) {
			savedLetter = -1;
			savedColumn = currentColumn;
			savedRow = currentRow;
			letter = caseLetter(letter, file);
			if (errorHappened) return AnalyzeResult(errorHappened, errorToken);
		}

		if (isOneSeparated(letter)) {
			savedLetter = -1;
			savedColumn = currentColumn;
			savedRow = currentRow;
			letter = caseOneSeparated(letter, file);
			if (errorHappened) return AnalyzeResult(errorHappened, errorToken);
		}

		if (isNumber(letter)) {
			savedLetter = -1;
			savedColumn = currentColumn;
			savedRow = currentRow;
			letter = caseNumber(letter, file);
			if (errorHappened) return AnalyzeResult(errorHappened, errorToken);
		}

		if (isComment(letter)) {
			savedLetter = -1;
			savedColumn = currentColumn;
			savedRow = currentRow;
			letter = caseComment(letter, file);
			if (errorHappened) return AnalyzeResult(errorHappened, errorToken);
		}
	
		if (isWhiteSpace(letter)) {
			savedColumn = currentColumn;
			savedRow = currentRow;
			savedLetter = -1;
			if (!token.empty()) {
				addToken(token);
				token = "";
			}
			resetTokenStatus();
			if (letter == '\n') {
				currentColumn = 0;
				currentRow++;
			}
			continue;
		}
		
		// to prevent endless loop when met undefined letter
		if (letter == savedLetter) {
			handleError("Undefined symbol", letter, currentRow, currentColumn);
			return AnalyzeResult(errorHappened, errorToken);
		}

		savedLetter = letter;

	}

	return AnalyzeResult(errorHappened, errorToken);

}

void Lexer::printScanResult() {
	for (auto& result : lexerResultValues) {
		result.print();
	}
}

std::vector<LexerResult> Lexer::getResults(){
	return lexerResultValues;
}
