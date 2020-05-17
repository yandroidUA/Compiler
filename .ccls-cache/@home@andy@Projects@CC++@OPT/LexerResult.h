#pragma once
#include <string>
#include <iostream>

class LexerResult
{
public:
	LexerResult(std::string token, int code, int rowNumber, int columnNumber, int indexInResultVector) {
		this->token = token;
		this->code = code;
		this->columnNumber = columnNumber;
		this->rowNumber = rowNumber;
		this->indexInResultVector = indexInResultVector;
	}

	~LexerResult() = default;

	inline std::string getToken() const { return token; }
	
	inline int getCode() const { return code; }

	inline int getRowNumber() const { return rowNumber; }

	inline int getColumnNumber() const { return columnNumber; }

	inline int getIndexInResultVector() const { return indexInResultVector; }

	void print() {
		// std::cout << token << ", code=" << code << ", column=" << columnNumber << ", row=" << rowNumber << std::endl;
		std::cout << rowNumber << " " << columnNumber << " " << code << " " << token << "position in resultVector: " << indexInResultVector << std::endl;
	}

private:
	std::string token;
	int code;
	int rowNumber;
	int columnNumber;
	int indexInResultVector;
};