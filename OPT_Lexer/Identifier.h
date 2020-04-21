#pragma once
#include <string>
#include "Attribute.h"

class Identifier {
public:

	Identifier(std::string name, int code, Attribute* attribute) {
		this->name = name;
		this->code = code;
		this->attribute = attribute;
	};

	~Identifier() = default;

	inline int getCode() const { return code; }

	inline std::string getName() const { return name; }

	inline Attribute::IdentifierType getType() { return attribute->getType(); }

	inline Attribute* getAttribute() const { return attribute; }

protected:
	int code;
	std::string name;
	Attribute* attribute;
};