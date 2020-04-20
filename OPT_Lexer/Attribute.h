#pragma once
#include <string>
#include "Identifier.h"

class Attribute {
public:
	Attribute(Identifier::IdentifierType type) {
		this->type = type;
	};

	virtual ~Attribute() = default;

	inline Identifier::IdentifierType getType() const { return type; }

protected:
	Identifier::IdentifierType type;
};