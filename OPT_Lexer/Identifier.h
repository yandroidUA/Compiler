#pragma once
#include <string>

class Identifier {
public:
	
	enum IdentifierType {
		INTEGER,
		FLOAT,
		RANGE,
		EMPTY
	};

	Identifier(std::string name, int code, IdentifierType type) {
		this->name = name;
		this->code = code;
		this->type = type;
	};

	~Identifier() = default;

	inline int getCode() const { return code; }

	inline std::string getName() const { return name; }

	inline IdentifierType getType() { return type; }

private:
	int code;
	std::string name;
	IdentifierType type;
};