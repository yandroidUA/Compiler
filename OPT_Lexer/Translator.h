#pragma once
#include "Tree.h"
#include "Identifier.h"
#include "Attribute.h"
#include "RangeAttribute.h"
#include "Expression.h"

class Translator {
public:
	// pass tree to translate as parametr to constructor
	Translator(Tree*);

	// param - is path to dump result file
	void analyze(std::string);

private:
	Tree* tree;
	int loopLabelCounter = 0;
	std::vector<Identifier> declaratedIdentifiers;

	// check if identififer with code param is contains in declaratedIdentifiers
	bool isIdentifierDeclarated(int);

	bool isIdentifierIsRange(int);

	bool isIdentifierIsInteger(int);

	void addIdentifier(Identifier);

	void analyze(Tree::TreeItem*, std::string&);

	// starting DATA SEGEMNT
	void startDataSegment();

	// ending DATA SEGMENT
	void endDataSegment();

	// start CODE SEGMENT
	void startCodeSegment();

	// end CODE SEGMENT
	void endCodeSegment();

	void caseProgram(Tree::TreeItem*);

	void caseProcedureIdentifier(Tree::TreeItem*);

	void caseBlock(Tree::TreeItem*);

	bool caseVariableDeclarations(Tree::TreeItem*);

	bool caseDeclaration(Tree::TreeItem*);

	bool caseDeclarationList(Tree::TreeItem*);

	bool caseStatementList(Tree::TreeItem*);

	bool caseStatement(Tree::TreeItem*);

	bool caseStatementLoop(Tree::TreeItem*);

	bool caseStatementExpression(Tree::TreeItem*);

	Expression* caseExpression(Tree::TreeItem*, bool);

	Expression* caseDimension(Tree::TreeItem*);

	Expression* caseVariable(Tree::TreeItem*, bool);

	Tree::TreeItem* caseVariableIdentifier(Tree::TreeItem*, bool);

	Tree::TreeItem* caseIdentifier(Tree::TreeItem*, bool);

	Attribute* caseAttribtue(Tree::TreeItem*);

	Attribute* caseAttributeList(Tree::TreeItem*);

	RangeAttribute* caseRange(Tree::TreeItem*);

	Tree::TreeItem* caseUnsignedInteger(Tree::TreeItem*);

	Identifier::IdentifierType convertFromCode(int, bool);

};

