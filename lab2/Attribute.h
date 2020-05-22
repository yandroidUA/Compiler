#pragma once
#include <string>

class Attribute {
public:
	enum IdentifierType {
		INTEGER,
		FLOAT,
		RANGE,
		EMPTY
	};

	Attribute(IdentifierType type) {
		this->type = type;
	};

	virtual ~Attribute() = default;

	inline IdentifierType getType() { return type; }

protected:
	IdentifierType type;
};