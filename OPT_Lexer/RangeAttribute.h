#pragma once
#include "Attribute.h"

class RangeAttribute : public Attribute {
public:
	RangeAttribute(int from, int to) : Attribute(Identifier::IdentifierType::RANGE) {
		this->from = from;
		this->to = to;
	};

	~RangeAttribute() = default;

	inline int getFrom() const { return from; }

	inline int getTo() const { return to; }

private:
	int from;
	int to;
};