#pragma once
#include "Tree.h"


class Translator {
public:
	// pass tree to translate as parametr to constructor
	Translator(Tree*);

	// param - is path to dump result file
	void analyze(std::string);

private:
	Tree* tree;
	int loopLabelCounter = 0;
	std::vector<int> declaratedIdentifiers;

	// check if identififer with code param is contains in declaratedIdentifiers
	bool isIdentifierDeclarated(int);

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

	Tree::TreeItem* caseExpression(Tree::TreeItem*);

	Tree::TreeItem* caseDimension(Tree::TreeItem*);

	Tree::TreeItem* caseVariable(Tree::TreeItem*);

	Tree::TreeItem* caseVariableIdentifier(Tree::TreeItem*);

	Tree::TreeItem* caseIdentifier(Tree::TreeItem*);

	Tree::TreeItem* caseAttribtue(Tree::TreeItem*);

	Tree::TreeItem* caseAttributeList(Tree::TreeItem*);

	// returns new TreeItem that has form and to as child
	// [ from .. to ]; from & to type of unsigned-integer
	// child[0] = from; child[1] = to
	Tree::TreeItem* caseRange(Tree::TreeItem*);

	Tree::TreeItem* caseUnsignedInteger(Tree::TreeItem*);

};

