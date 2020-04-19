#include "Translator.h"

std::string dataSegmentString = "";
std::string codeSegmentString = "";

Translator::Translator(Tree* tree) {
	this->tree = tree;
}

void Translator::analyze(std::string) {
	tree->switchTo(tree->getRoot());
//	startCodeSegment();
//	startDataSegment();
	analyze(tree->getRoot(), dataSegmentString);
}

bool Translator::isIdentifierDeclarated(int identifierCode) {
	return std::find(declaratedIdentifiers.begin(), declaratedIdentifiers.end(), identifierCode) != declaratedIdentifiers.end();
}

void Translator::analyze(Tree::TreeItem* item, std::string& stringToWrite) {
	if (item == nullptr) return;
	switch (item->getRule()) {
	case PROGRAM:
		 caseProgram(item);
		 break;
	default:
		for (auto& child : item->getChilds()) {
			analyze(child, stringToWrite);
		}
		break;
	}
}

void Translator::caseProgram(Tree::TreeItem* item) {
	std::vector<Tree::TreeItem*> programChilds = item->getChilds();
	if (programChilds.size() != 2) {
		std::cout << "Error! Program must have <procedure-identifier> AND <block>";
		return;
	}

	caseProcedureIdentifier(programChilds.at(0));
	caseBlock(programChilds.at(1));
}

void Translator::caseProcedureIdentifier(Tree::TreeItem* item) {
	std::vector<Tree::TreeItem*> procedureIdentifierChilds = item->getChilds();
	if (procedureIdentifierChilds.size() != 1 || procedureIdentifierChilds.at(0)->getRule() != IDENTIFIER_RULE) {
		std::cout << "Error! <procedure-identifier> must contains IDENTIFIER" << std::endl;
		return;
	}

	//TODO: add more checks
	Tree::TreeItem* identifier = procedureIdentifierChilds.at(0)->getChilds().at(0);
	std::cout << "PROC " << identifier->getStringData() << std::endl;
}

void Translator::caseBlock(Tree::TreeItem* item) {
	std::vector<Tree::TreeItem*> blockChilds = item->getChilds();

	if (blockChilds.size() != 4) {
		std::cout << "ERROR! <block> MUST have 4 childs: <variable-declarations>, BEGIN, <statement-list>, END, but got " << blockChilds.size() << std::endl;
		return;
	}

	if (!caseVariableDeclarations(blockChilds.at(0))) {
		std::cout << "ERROR! <variable-declarations> error happaned!" << std::endl;
		return;
	}

	if (blockChilds.at(1)->getData() != 402) {
		std::cout << "ERROR! BEGIN expected, but got " << blockChilds.at(1)->getStringData() << std::endl;
		return;
	}

	//TODO: here generate CODE_SEGMENT
	startCodeSegment();
	caseStatementList(blockChilds.at(2));

}

bool Translator::caseVariableDeclarations(Tree::TreeItem* item) {
	if (item->getRule() != VARIABLE_DECLARATIONS) {
		std::cout << "ERROR! <variable-declarations> expected, but got " << item->getRule() << std::endl;
		return false;
	}

	std::vector<Tree::TreeItem*> variableDeclarationsChild = item->getChilds();

	if (variableDeclarationsChild.size() != 2) {
		std::cout << "ERROR! <variable-declarations> must have only 2 childs, but got " << variableDeclarationsChild.size() << std::endl;
		return false;
	}

	// checking if 1st child of variableDeclarationsChild is VAR
	if (variableDeclarationsChild.at(0)->getData() != 404) {
		std::cout << "VAR(404) expected, but got " << variableDeclarationsChild.at(0)->getData() << std::endl;
		return false;
	}

	startDataSegment();
	if (variableDeclarationsChild.at(1)->getRule() == EMPTY) {
		return true;
	}

	return caseDeclarationList(variableDeclarationsChild.at(1));
}

bool Translator::caseDeclaration(Tree::TreeItem* item) {
	if (item->getRule() != DECLARATION) {
		std::cout << "ERROR! DECLARATION expected, but got " << item->getRule() << std::endl;
		return false;
	}

	std::vector<Tree::TreeItem*> declarationChild = item->getChilds();

	if (declarationChild.size() < 4) {
		std::cout << "ERROR! DECLARATIONS_LIST MUST have at least 4 childs, but got " << declarationChild.size() << std::endl;
		return false;
	}

	Tree::TreeItem* variable = caseVariableIdentifier(declarationChild.at(0));
	if (variable == nullptr) return false;

	if (declarationChild.at(1)->getData() != 0) {
		std::cout << "ERROR! Expected ':'(0), but got " << declarationChild.at(1)->getStringData() << std::endl;
		return false;
	}

	Tree::TreeItem* attribute = caseAttribtue(declarationChild.at(2));
	if (attribute == nullptr) return false;

	Tree::TreeItem* attributeList = caseAttributeList(declarationChild.at(3));
	if (attributeList == nullptr) return false;

	attribute = attributeList->getRule() == EMPTY ? attribute : attributeList;

	//TODO: GENERATE HERE DECLARATION ASM
	std::cout << variable->getStringData() << " " << attribute->getStringData() << " ?" << std::endl;
	return true; 
}

bool Translator::caseDeclarationList(Tree::TreeItem* item) {
	if (item->getRule() != DECLARATIONS_LIST) {
		std::cout << "ERROR! Expected DECLARATIONS_LIST, but got " << item->getRule() << std::endl;
		return false;
	}

	std::vector<Tree::TreeItem*> declarationListChilds = item->getChilds();

	if (declarationListChilds.size() == 1) {
		if (declarationListChilds.at(0)->getRule() == EMPTY) {
			std::cout << "nop" << std::endl;
			return true;
		} else {
			std::cout << "ERROR! <declaration-list> with 1 child MUST have EMPTY child" << std::endl;
			return false;
		}
	}

	if (declarationListChilds.size() == 2) {
		bool isDeclaration = caseDeclaration(declarationListChilds.at(0));
		return isDeclaration ? caseDeclarationList(declarationListChilds.at(1)) : isDeclaration;
	} else {
		std::cout << "ERROR! <declaration-list> must have 1 or 2 childs, but got " << declarationListChilds.size() << std::endl;
		return false;
	}
}

bool Translator::caseStatementList(Tree::TreeItem* item) {
	if (item->getRule() != STATEMENT_LIST) {
		std::cout << "ERROR! Expected <statement-list>, but got " << item->getRule() << std::endl;
		return false;
	}

	std::vector<Tree::TreeItem*> statementListChild = item->getChilds();

	if (statementListChild.size() == 1) {
		if (statementListChild.at(0)->getRule() == EMPTY) {
			std::cout << "nop" << std::endl;
			return true;
		} else {
			std::cout << "ERROR! If <statement-list> has 1 child it MUST be EMPTY, but got " << statementListChild.at(0)->getRule() << std::endl;
			return false;
		}
	}

	if (statementListChild.size() != 2) {
		std::cout << "ERROR! <statement-list> MUST have 1 or 2 childs, but got " << statementListChild.size() << std::endl;
		return false;
	}
	
	bool isStatement = caseStatement(statementListChild.at(0));

	return isStatement ? caseStatementList(statementListChild.at(1)) : isStatement;
}

bool Translator::caseStatement(Tree::TreeItem* item) {
	if (item->getRule() != STATEMENT) {
		std::cout << "ERROR! STATEMENT expected, but got " << item->getRule() << std::endl;
		return false;
	}

	std::vector<Tree::TreeItem*> statementChilds = item->getChilds();

	if (statementChilds.size() != 4) {
		std::cout << "ERROR! <statement-list> MUST have 4 child items, but got " << statementChilds.size() << std::endl;
		return false;
	}

	// LOOP
	if (statementChilds.at(0)->getData() == 406) {
		return caseStatementLoop(item);
	}
	
	//expression
	return caseStatementExpression(item);
}

bool Translator::caseStatementLoop(Tree::TreeItem* item) {
	int currentLoopCounter = loopLabelCounter;
	std::cout << "?L" << loopLabelCounter << ": nop" << std::endl;
	loopLabelCounter++;

	std::vector<Tree::TreeItem*> statementLoopChilds = item->getChilds();

	bool isStatemntList = caseStatementList(statementLoopChilds.at(1));
	if (isStatemntList == false) return false;

	if (statementLoopChilds.at(2)->getData() != 407) {
		std::cout << "ERROR! ENDLOOP expected, but got " << statementLoopChilds.at(2)->getStringData() << std::endl;
		return false;
	}
	
	std::cout << "JMP ?L" << currentLoopCounter << std::endl;
	std::cout << "?L" << loopLabelCounter << ": nop" << std::endl;
	loopLabelCounter++;

	return true;
}

bool Translator::caseStatementExpression(Tree::TreeItem* item) {
	std::vector<Tree::TreeItem*> statementExpressionsChild = item->getChilds();

	Tree::TreeItem* variable = caseVariable(statementExpressionsChild.at(0));

	if (statementExpressionsChild.at(1)->getData() != 301) {
		std::cout << "ERROR! := expected, but got " << statementExpressionsChild.at(1)->getStringData() << std::endl;
		return false;
	}

	//TODO: add [3] item must be ;
	Tree::TreeItem* expression = caseExpression(statementExpressionsChild.at(2));
	return true;
}

Tree::TreeItem* Translator::caseExpression(Tree::TreeItem* item) {
	if (item->getRule() != EXPRESSION) {
		std::cout << "ERROR! EXPRESSION expected, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> expressionChilds = item->getChilds();

	if (expressionChilds.size() != 1) {
		std::cout << "ERROR! <expression> must have 1 child, but got " << expressionChilds.size() << std::endl;
		return nullptr;
	}

	switch (expressionChilds.at(0)->getRule()) {
	case UNSIGNED_INTEGER: return caseUnsignedInteger(expressionChilds.at(0));
	case VARIABLE: return caseVariable(expressionChilds.at(0));
	default: 
		std::cout << "ERROR! UNSIGNED_INTEGER or VARIABLE expected, but got " << expressionChilds.at(0)->getRule() << std::endl;
		return nullptr;
	}
}

Tree::TreeItem* Translator::caseDimension(Tree::TreeItem* item) {
	if (item->getRule() != DIMENSION) {
		std::cout << "ERROR! DIMENSION expected, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> diemnsionChilds = item->getChilds();

	if (diemnsionChilds.size() == 1) {
		if (diemnsionChilds.at(0)->getRule() == EMPTY) {
			return diemnsionChilds.at(0);
		} else {
			std::cout << "ERROR! if size of <dimension> is 1 it must be EMPTY" << std::endl;
			return nullptr;
		}
	}

	if (diemnsionChilds.size() != 3) {
		std::cout << "ERROR! <diemnsion> MUST have 1 or 3 childs, but got " << diemnsionChilds.size() << std::endl;
		return nullptr;
	}

	//TODO: here check 0 & 2 childs

	return caseExpression(diemnsionChilds.at(1));
}

Tree::TreeItem* Translator::caseVariable(Tree::TreeItem* item) {
	if (item->getRule() != VARIABLE) {
		std::cout << "ERROR! VARIABLE expected, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> variableChilds = item->getChilds();

	if (variableChilds.size() < 2) {
		std::cout << "ERROR! <variable> MUST have at least 2 childs, but got " << variableChilds.size();
		return nullptr;
	}

	Tree::TreeItem* variableIdentifier = caseVariableIdentifier(variableChilds.at(0));
	if (variableIdentifier == nullptr) return nullptr;

	//TODO: somehow convert to Tree:TreeItem this result and than parse it
	// maybe add some functions to encypt this expression-with-dimension to Tree::TreeItem and decrypt again
	// and add to Rule.h EXPRESSION_WITH_DIMENSSION
	Tree::TreeItem* dimension = caseDimension(variableChilds.at(1));
	//TODO: do here something
	return nullptr;
}

Tree::TreeItem* Translator::caseVariableIdentifier(Tree::TreeItem* item) {
	if (item->getRule() != VARIABLE_IDENTIFIER) {
		std::cout << "ERROR! Expected VARIABLE_IDENTIFIER, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> variableIdentifierChilds = item->getChilds();
	if (variableIdentifierChilds.size() != 1) {
		std::cout << "ERROR! <variable-identifier> MUST have only 1 child, but got" << variableIdentifierChilds.size() << std::endl;
		return nullptr;
	}

	return caseIdentifier(variableIdentifierChilds.at(0));
}

Tree::TreeItem* Translator::caseIdentifier(Tree::TreeItem* item) {
	if (item->getRule() != IDENTIFIER_RULE) {
		std::cout << "ERROR! Expected IDENTIFIER_RULE, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> identifierRuleChilds = item->getChilds();

	if (identifierRuleChilds.size() != 1) {
		std::cout << "ERROR! IDENTIFIER_RULE MUST have only 1 child, but got " << identifierRuleChilds.size() << std::endl;
		return nullptr;
	}

	Tree::TreeItem* identifier = identifierRuleChilds.at(0);
	if (identifier->getRule() != ADDING_INDENTIFIER) {
		std::cout << "ERROR! Expected ADDING_INDENTIFIER, but got " << identifier->getRule() << std::endl;
		return nullptr;
	}

	return identifier;
}

// TODO: when use this method need check the RULE of returned Tree::TreeItem if it;s RANGE -> need to parse Range and generate ASM
Tree::TreeItem* Translator::caseAttribtue(Tree::TreeItem* item) {
	if (item->getRule() != ATTRIBUTE) {
		std::cout << "ERROR! ATTRIBUTE expected, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> attributeChilds = item->getChilds();

	if (attributeChilds.size() == 1) {
		if (attributeChilds.at(0)->getData() == 405 || attributeChilds.at(0)->getData() == 408) {
			return attributeChilds.at(0);
		} else {
			std::cout << "ERROR! <attribute> with 1 child must have INTEGER or FLOAT as chold, but got " << attributeChilds.at(0)->getRule() << std::endl;
			return nullptr;
		}
	}

	if (attributeChilds.size() != 3) {
		std::cout << "ERROR! <attribute> MUST have 1 or 3 childs, but got " << attributeChilds.size() << std::endl;
		return nullptr;
	}
	
	return caseRange(attributeChilds.at(1));
}

Tree::TreeItem* Translator::caseAttributeList(Tree::TreeItem* item) {
	if (item->getRule() != ATTRIBUTES_LIST) {
		std::cout << "ERROR! <attribute-list> expected, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> attributeListChilds = item->getChilds();

	if (attributeListChilds.size() == 1) {
		if (attributeListChilds.at(0)->getRule() == EMPTY) {
			return attributeListChilds.at(0);
		} else {
			std::cout << "ERROR! <attribute-list> with childs count 1 MUST have only EMPTY child" << std::endl;
			return nullptr;
		}
	}

	if (attributeListChilds.size() != 2) {
		std::cout << "ERROR! <attribute-list> MUST contains 2 childs" << std::endl;
		return nullptr;
	}

	Tree::TreeItem* attribute = caseAttribtue(attributeListChilds.at(0));
	if (attribute == nullptr) return nullptr;

	Tree::TreeItem* attributeList = caseAttributeList(attributeListChilds.at(1));

	return attributeList->getRule() == EMPTY ? attribute : attributeList;
}

Tree::TreeItem* Translator::caseRange(Tree::TreeItem* item) {
	if (item->getRule() != RANGE) {
		std::cout << "ERROR! Eexpected RANGE, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> rangeChilds = item->getChilds();
	if (rangeChilds.size() != 3) {
		std::cout << "ERROR! <range> MUST have 3 childs, but got " << rangeChilds.size() << std::endl;
		return nullptr;
	}

	Tree::TreeItem* from = caseUnsignedInteger(rangeChilds.at(0));
	if (from == nullptr) return nullptr;

	Tree::TreeItem* divider = rangeChilds.at(1);
	if (divider->getData() != 302) {
		std::cout << "ERROR! Expected .., but got " << divider->getStringData() << std::endl;
		return nullptr;
	}

	Tree::TreeItem* to = caseUnsignedInteger(rangeChilds.at(2));
	if (to == nullptr) return nullptr;
	//TODO: add childs as i need
	std::string str = from->getStringData() + " " + divider->getStringData() + " " + to->getStringData();
	return new Tree::TreeItem(str, RANGE, -1);
}

Tree::TreeItem* Translator::caseUnsignedInteger(Tree::TreeItem* item) {
	if (item->getRule() != UNSIGNED_INTEGER) {
		std::cout << "ERROR! Expected UNSIGNED_INTEGER, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> unsignedIntegerChilds = item->getChilds();

	if (unsignedIntegerChilds.size() != 1) {
		std::cout << "ERROR! <unsigned-integer> MUST have 1 child, but got " << unsignedIntegerChilds.size() << std::endl;
		return nullptr;
	}

	Tree::TreeItem* integer = unsignedIntegerChilds.at(0);
	if (integer->getRule() != ADDING_CONSTANT) {
		std::cout << "ERROR! Expected CONSTANT, but got " << integer->getRule() << std::endl;
		return nullptr;
	}

	return integer;
}

void Translator::startDataSegment(){
	std::cout << "DATA SEGMENT" << std::endl;
	dataSegmentString = "DATA SEGMENT\n";
}

void Translator::endDataSegment(){
	std::cout << "DATA ENDS" << std::endl;
	dataSegmentString += "DATA ENDS\n";
}

void Translator::startCodeSegment(){
	std::cout << "CODE SEGMENT" << std::endl;
	std::cout << "ASSUME CS:CODE DS:DATA" << std::endl;
	codeSegmentString = "CODE SEGMENT\n";
	codeSegmentString += "ASSUME CS:CODE DS:DATA\n";
}

void Translator::endCodeSegment(){
	std::cout << "CODE ENDS" << std::endl;
	codeSegmentString += "CODE ENDS\n";
}
