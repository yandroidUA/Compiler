#pragma once
#include <string>

class Variable {
public:

	Variable(std::string name, int code, int type) {
		this->name = name;
		this->code = code;
		this->type = type;
		this->dimensionPart = nullptr;
	}

	Variable(std::string name, int code, int type, Variable* dimensionPart) {
		this->name = name;
		this->code = code;
		this->type = type;
		this->dimensionPart = dimensionPart;
	}

	~Variable() = default;

	inline std::string getName() const { return name; }

	inline int getCode() const { return code; }

	inline int getType() const { return type; }

	inline Variable* getDimensionPart() { return dimensionPart; }

private:
	std::string name;
	int code;
	int type;
	Variable* dimensionPart;
};