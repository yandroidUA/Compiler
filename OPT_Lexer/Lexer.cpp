#include "Lexer.h"
#include <string>
#include <iostream>

typedef std::pair<std::string, int> MapPair;

int TOKEN_STATUS_CONSTANT = 0;
int TOKEN_STATUS_IDENTIFIER = 1;
int TOKEN_STATUS_RESERVED_WORD = 2;
int TOKEN_STATUS_ONE_SEPARATED_TOKEN = 3;
int TOKEN_STATUS_MULTI_SEPARATED_TOKEN = 4;

int oneSymbolTokenIndex = 0;
int multiSymbolTokenIndex = 301;
int reservedWordsIndex = 401;
int constantsIndex = 501;
int variablesIndex = 1001;

int savedMultiSeparatedToken = -1;

int currentTokenState[5] = {0, 0, 0, 0, 0};
std::string token = "";

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
		if (number == character) {
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
}

Lexer::Lexer() {
	addReservedWord("PROGRAM");
	addReservedWord("BEGIN");
	addReservedWord("END");
	addReservedWord("VAR");
	addReservedWord("FLOAT");
	addReservedWord("LOOP");
	addReservedWord("ENDLOOP");
	addReservedWord("..");

	addOneSeparatedToken(":");
	addOneSeparatedToken(";");
	
	addMultiSeparatedToken(":=");
}

void Lexer::addToken(std::string&) {
	if (currentTokenState[TOKEN_STATUS_CONSTANT] == 1) {
		std::cout << token << " is constant" << std::endl;
		return;
	}

	if (currentTokenState[TOKEN_STATUS_IDENTIFIER] == 1 && currentTokenState[TOKEN_STATUS_RESERVED_WORD] == 0) {
		std::cout << token << " is identifier" << std::endl;
		return;
	}

	if (currentTokenState[TOKEN_STATUS_RESERVED_WORD] == 1 && currentTokenState[TOKEN_STATUS_IDENTIFIER] == 0) {
		std::cout << token << " is reserved word" << std::endl;
		return;
	}

	if (currentTokenState[TOKEN_STATUS_ONE_SEPARATED_TOKEN] == 1) {
		std::cout << token << " is one separated word" << std::endl;
		return;
	}

	if (currentTokenState[TOKEN_STATUS_MULTI_SEPARATED_TOKEN] == 1) {
		std::cout << token << " is multi separated word" << std::endl;
		return;
	}

	int reservedWordCode = isTokenReservedWord(token);
	if (reservedWordCode != -1) {
		std::cout << token << " is reserved word" << std::endl;
		return;
	} else {
		std::cout << token << " is identifier" << std::endl;
		return;
	}

}

void Lexer::addReservedWord(const char* word) {
	reservedWords.insert(MapPair(word, reservedWordsIndex));
	reservedWordsIndex++;
}

void Lexer::addOneSeparatedToken(const char* word) {
	oneSeparatedTokensMap.insert(MapPair(word, oneSymbolTokenIndex));
	oneSymbolTokenIndex++;
}

void Lexer::addMultiSeparatedToken(const char* word) {
	multiSeparatedTokensMap.insert(MapPair(word, multiSymbolTokenIndex));
	multiSymbolTokenIndex++;
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

void Lexer::scanFile(const char* filePath) {
	std::ifstream file;

	file.open(filePath, std::ios::in);
	int letter = readCharacterFromFile(file);
	analyzeLetter(letter, file);
	file.close();
}

int Lexer::readCharacterFromFile(std::ifstream &file) {
	char currentCaracter = file.eof();
	if (!file.eof()) {
		file.get(currentCaracter);
	}

	return (int)currentCaracter;
}

void Lexer::analyzeLetter(int letter, std::ifstream &file) {

	if (isOneSeparated(letter) && currentTokenState[TOKEN_STATUS_MULTI_SEPARATED_TOKEN] == 0) {
		if (!token.empty()) {
			addToken(token);
		}
		resetTokenStatus();
		currentTokenState[TOKEN_STATUS_ONE_SEPARATED_TOKEN] = 1;
		if (isMultiSeparated(letter)) {
			currentTokenState[TOKEN_STATUS_MULTI_SEPARATED_TOKEN] = 1;
			savedMultiSeparatedToken = letter;
		}
	}

	if (savedMultiSeparatedToken != -1 && isSecondPartOfMultiSeparated(savedMultiSeparatedToken, letter) && currentTokenState[TOKEN_STATUS_MULTI_SEPARATED_TOKEN] == 1) {
		currentTokenState[TOKEN_STATUS_ONE_SEPARATED_TOKEN] = 0;
	}
		 
	if (isNumber(letter)) {
		if (currentTokenState[TOKEN_STATUS_IDENTIFIER] != 1) {
			resetTokenStatus();
			currentTokenState[TOKEN_STATUS_CONSTANT] = 1;
		} else {
			currentTokenState[TOKEN_STATUS_RESERVED_WORD] = 0;
		}

	}

	if (isLetter(letter)) {
		if (currentTokenState[TOKEN_STATUS_CONSTANT] == 1) {
			std::cout << "Error constant cannot contains letter" << std::endl;
			return;
		}
		currentTokenState[TOKEN_STATUS_IDENTIFIER] = 1;
		currentTokenState[TOKEN_STATUS_RESERVED_WORD] = 1;
	}

	if (isWhiteSpace(letter)) {
		if (!token.empty()) {
			addToken(token);
			resetTokenStatus();
			token = "";
		}
	} else {
		token += (char)letter;
	}

	
	int nextLetter = readCharacterFromFile(file);
	if (nextLetter != file.eof()) {
		analyzeLetter(nextLetter, file);
	}
}
