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

	void analyze();

	void dumpIntoFile(std::string);

private:
	Tree* tree;
	int loopLabelCounter = 0;
	std::string assemblerProgram = "";
	std::string errorString = "";
	bool errorHappened = false;
	std::string procedureName;
	std::vector<Identifier> declaratedIdentifiers;

	// Print error into console and set errorHappened to TRUE
	void handleError(std::string);

	// same as handleError, but add information about position and value
	void handleError(std::string, Tree::TreeItem*);

	// check if identififer with code param is contains in declaratedIdentifiers
	bool isIdentifierDeclarated(int);

	bool isIdentifierIsRange(int);

	bool isIdentifierIsInteger(int);

	Identifier* getIdentifier(int);

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

	bool caseProgram(Tree::TreeItem*);

	bool caseProcedureIdentifier(Tree::TreeItem*);

	bool caseBlock(Tree::TreeItem*);

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

	void generateAsm(Identifier*);

	void generateAsm(Expression*, Expression*);

	std::string generatePrepareLeftExpression(Expression*, bool);
	
	std::string generatePrepareRightExpression(Expression*, bool);
		
};

