#pragma once
class MultiSeparatedToken
{
public:
	
	MultiSeparatedToken(int _code, int _pairCode) {
		code = _code;
		pairCode = _pairCode;
	}

	~MultiSeparatedToken() = default;

	inline int getCode() { return code; }
	inline int getPairCode() { return pairCode; }

private:
	int code;
	int pairCode;
};
