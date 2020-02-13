#pragma once
#include <vector>
#include <fstream>
#include <map>
#include "MultiSeparatedToken.h"

class Lexer {
private:

	std::vector<int> lettersVector = {
		'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', 'g', 'G', 'h', 'H',
		'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N', 'o', 'O', 'p', 'P', 'q', 'Q', 'r', 'R',
		's', 'S', 't', 'T', 'u', 'U', 'v', 'V', 'w', 'W', 'x', 'X', 'y', 'y', 'z', 'Z'
	};
	std::vector<int> numbersVector = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	std::vector<int> whiteSpacesVector = {9, 10, 13, 32};
	std::vector<int> oneSeparatedTokens = {':', ';'};
	std::vector<MultiSeparatedToken> multiSeparatedTokens = {
		MultiSeparatedToken(':', '=')
	};

	std::map<std::string, int> reservedWords;
	std::map<std::string, int> multiSeparatedTokensMap;
	std::map<std::string, int> oneSeparatedTokensMap;

	bool isLetter(int);
	bool isNumber(int);
	bool isWhiteSpace(int);
	bool isOneSeparated(int);
	bool isMultiSeparated(int);
	bool isSecondPartOfMultiSeparated(int, int);

	void resetTokenStatus();
	void addToken(std::string&);
	void addReservedWord(const char*);
	void addOneSeparatedToken(const char*);
	void addMultiSeparatedToken(const char*);

	int isTokenReservedWord(std::string&);
	int isTokenOneSeparatedToken(std::string&);
	int isTokenMultiSeparatedToken(std::string&);

	void analyzeLetter(int, std::ifstream&);
	int readCharacterFromFile(std::ifstream&);

public:

	Lexer();

	void scanFile(const char* filePath);
};

