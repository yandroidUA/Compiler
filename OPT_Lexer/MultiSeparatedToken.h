#pragma once
class MultiSeparatedToken
{
public:
	MultiSeparatedToken(int, int);

	int getCode();
	int getPairCode();

private:
	int code;
	int pairCode;
};

MultiSeparatedToken::MultiSeparatedToken(int code, int pairCode) {
	this->code = code;
	this->pairCode = pairCode;
}

inline int MultiSeparatedToken::getCode() {
	return code;
}

inline int MultiSeparatedToken::getPairCode(){
	return pairCode;
}
