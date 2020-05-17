#pragma once
#include <string>
#include <iostream>

class Expression {
public:

	enum ExpressionType {
		UNSIGNED_INTEGER,
		VARIABLE,
		EMPTY
	};

	Expression(std::string name, int code, int line, int column, ExpressionType type) {
		this->name = name;
		this->code = code;
		this->type = type;
		this->line = line;
		this->column = column;
		this->dimension = nullptr;
	};

	Expression(std::string name, int code, int line, int column, ExpressionType type, Expression* dimension) {
		this->name = name;
		this->code = code;
		this->type = type;
		this->line = line;
		this->column = column;
		this->dimension = dimension;
	};

	inline ExpressionType getType() const { return type; }

	inline int getCode() const { return code; }

	inline int getLine() const { return line; }

	inline int getColumn() const { return column; }

	inline Expression* getDimension() { return dimension; }

	inline std::string getName() const { return name; }

	//TODO: delete this after debug
	void print() {
		std::cout << name;
		if (dimension == nullptr) return;
		std::cout << "[";
		dimension->print();
		std::cout << "]";
	}

	std::string getVariableForPrint() {
		if (dimension == nullptr) return name;
		return name + "[" + dimension->getVariableForPrint() + "]";
	}

private:
	std::string name;
	int code;
	int line;
	int column;
	ExpressionType type;
	Expression* dimension;
};

