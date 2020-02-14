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
		's', 'S', 't', 'T', 'u', 'U', 'v', 'V', 'w', 'W', 'x', 'X', 'y', 'y', 'z', 'Z', 'i', 'I'
	};
	std::vector<int> numbersVector = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
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
	bool isComment(int);

	void resetTokenStatus();
	void addToken(std::string&);
	void addReservedWord(const char*);
	void addOneSeparatedToken(const char*);
	void addMultiSeparatedToken(const char*);

	int isTokenReservedWord(std::string&);
	int isTokenOneSeparatedToken(std::string&);
	int isTokenMultiSeparatedToken(std::string&);

	int readCharacterFromFile(std::ifstream&);

	// each of this methods will return ASCII code of letter that don't match case
	int caseOneSeparated(int, std::ifstream&);
	int caseMultiSeparated(int, std::ifstream&);
	int caseLetter(int, std::ifstream&);
	int caseNumber(int, std::ifstream&);
	int caseComment(int, std::ifstream&);

public:

	Lexer();

	void scanFileNotRecyrsivly(const char*);
};

