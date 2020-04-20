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

	Expression(std::string name, int code, ExpressionType type) {
		this->name = name;
		this->code = code;
		this->type = type;
		this->dimension = nullptr;
	};

	Expression(std::string name, int code, ExpressionType type, Expression* dimension) {
		this->name = name;
		this->code = code;
		this->type = type;
		this->dimension = dimension;
	};

	inline ExpressionType getType() const { return type; }

	inline int getCode() const { return code; }

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

private:
	std::string name;
	int code;
	ExpressionType type;
	Expression* dimension;
};

