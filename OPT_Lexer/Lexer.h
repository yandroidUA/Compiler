#pragma once
#include <vector>
#include <fstream>
#include <map>
#include "MultiSeparatedToken.h"
#include "LexerResult.h"
#include "TokenStatus.h"

class Lexer {
private:
	
	std::vector<int> lettersVector = {
		'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', 'g', 'G', 'h', 'H',
		'k', 'K', 'l', 'L', 'm', 'M', 'n', 'N', 'o', 'O', 'p', 'P', 'q', 'Q', 'r', 'R',
		's', 'S', 't', 'T', 'u', 'U', 'v', 'V', 'w', 'W', 'x', 'X', 'y', 'y', 'z', 'Z', 
		'i', 'I'
	};
	std::vector<int> numbersVector = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
	std::vector<int> whiteSpacesVector = {9, 10, 13, 32};
	std::vector<int> oneSeparatedTokens = {':', ';', '[',']','.'};
	std::vector<MultiSeparatedToken> multiSeparatedTokens = {
		MultiSeparatedToken(':', '='),
		MultiSeparatedToken('.', '.')
	};
	std::vector<LexerResult> lexerResultValues;

	std::map<std::string, int> reservedWords;
	std::map<std::string, int> multiSeparatedTokensMap;
	std::map<std::string, int> oneSeparatedTokensMap;
	std::map<std::string, int> identifiersTokensMap;
	std::map<std::string, int> constantTokensMap;

	// search ASCII code of param in lettersVector and if found return TRUE else FALSE
	bool isLetter(int);
	// search ASCII code of param in numbersVector and if found return TRUE else FALSE
	bool isNumber(int);
	// return TRUE if param is '('
	bool isComment(int);
	// search ASCII code of param in whiteSpacesVector and if found return TRUE else FALSE
	bool isWhiteSpace(int);
	// search ASCII code of param in oneSeparatedTokens and if found return TRUE else FALSE
	bool isOneSeparated(int);
	// search ASCII code of param in multiSeparatedTokens and if found return TRUE else FALSE
	bool isMultiSeparated(int);
	// search ASCII code of param in multiSeparatedTokens and if found return TRUE else FALSE
	bool isSecondPartOfMultiSeparated(int, int);

	// set status of token to 0
	void resetTokenStatus();
	// check status of token and depends on value add token as: ReservedWord, Identifier, OneSeparatedToken or MultiSeparatedToken
	// invoke addTokenToResult to add token to result vector
	void addToken(std::string&);
	// add param of func to reservedWords
	void addReservedWord(const char*);
	// add param of func to oneSeparatedTokensMap
	void addOneSeparatedToken(const char*);
	// add param of func to multiSeparatedTokensMap
	void addMultiSeparatedToken(const char*);
	// add token to identifiersTokensMap, returns code of just added identifier
	int addIdentifier(std::string&);
	// add token to constantTokensMap, returns code of just added constant
	int addConstant(std::string&);

	// add token to ResultsVector
	void addTokenToResultVector(std::string&, TokenStatus, int, int);

	// check if passed token (param of func) is contains in: _ and return code if contains and -1 if not
	// _ = reservedWords
	int isTokenReservedWord(std::string&);
	// _ = oneSeparatedTokensMap
	int isTokenOneSeparatedToken(std::string&);
	// _ = multiSeparatedTokensMap
	int isTokenMultiSeparatedToken(std::string&);
	// _ = identifiersTokensMap
	int isTokenIdentifier(std::string&);
	// _ = constantTokensMap
	int isTokenConstant(std::string&);

	// return ASCII code of next character from file
	int readCharacterFromFile(std::ifstream&);

	// params of func is OneSeparated token
	// this func check if param can be MultiSeparatedToken and if it is invoke caseMultiSeparated
	// returns ASCII code of NOT letter OneSeparated, but before invoke addToken and add scanned token
	int caseOneSeparated(int, std::ifstream&);
	// params of func is OneSeparated token
	// this func check if param can be MultiSeparatedToken and if it is invoke readCharacterFromFile
	// if readed character is pair for MultiSeparatedToken add token and return nextLetter using readCharacterFromFile
	// returns ASCII code of NOT MultiSeparated letter, but before invoke addToken and add scanned token
	int caseMultiSeparated(int, std::ifstream&);
	// params of func is letter token
	// this func check if param can be ReservedWord or Ientifier
	// returns ASCII code of NOT letter letter, but before invoke addToken and add scanned token
	int caseLetter(int, std::ifstream&);
	// params of func is letter token
	// this func check next letter and if it isNumber == true add value to token, else
	// returns ASCII code of NOT number letter, but before invoke addToken and add scanned token
	int caseNumber(int, std::ifstream&);
	// params of func is '(' letter
	// check nextLetter and if it is not '*' print ERROR
	// else scan next letter untill ['*',')'] will be scanned one by one
	// returns ASCII code of letter that don't in comment block, but before invoke addToken and add scanned token
	int caseComment(int, std::ifstream&);

public:

	Lexer();
	~Lexer() = default;

	void scanFile(const char*);
	void printScanResult();
	std::vector<LexerResult> getResults();
};
