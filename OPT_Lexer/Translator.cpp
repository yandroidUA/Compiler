#include "Translator.h"
#include "ReservedWords.h"
#include "Identifier.h"

std::string dataSegmentString = "";
std::string codeSegmentString = "";

Translator::Translator(Tree* tree) {
	this->tree = tree;
}

void Translator::analyze() {
	tree->switchTo(tree->getRoot());
	analyze(tree->getRoot(), dataSegmentString);
}

bool Translator::isIdentifierDeclarated(int identifierCode) {
	for (auto& identifier : declaratedIdentifiers) {
		if (identifier.getCode() == identifierCode) {
			return true;
		}
	}

	return false;
}

bool Translator::isIdentifierIsRange(int code) {
	for (auto& identifier : declaratedIdentifiers) {
		if (identifier.getCode() == code) {
			return identifier.getType() == Attribute::IdentifierType::RANGE;
		}
	}
	return false;
}

bool Translator::isIdentifierIsInteger(int code) {
	for (auto& identifier : declaratedIdentifiers) {
		if (identifier.getCode() == code) {
			return identifier.getType() == Attribute::IdentifierType::INTEGER;
		}
	}
	return false;
}

Identifier* Translator::getIdentifier(int code) {
	for (auto& identifier : declaratedIdentifiers) {
		if (identifier.getCode() == code) {
			return &identifier;
		}
	}
	return nullptr;
}

void Translator::addIdentifier(Identifier identifier) {
	declaratedIdentifiers.push_back(identifier);
}

void Translator::analyze(Tree::TreeItem* item, std::string& stringToWrite) {
	if (item == nullptr) return;
	switch (item->getRule()) {
	case PROGRAM_RULE:
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
	std::cout << "END" << std::endl;
}

void Translator::caseProcedureIdentifier(Tree::TreeItem* item) {
	std::vector<Tree::TreeItem*> procedureIdentifierChilds = item->getChilds();
	if (procedureIdentifierChilds.size() != 1 || procedureIdentifierChilds.at(0)->getRule() != IDENTIFIER_RULE) {
		std::cout << "Error! <procedure-identifier> must contains IDENTIFIER" << std::endl;
		return;
	}

	//TODO: add more checks
	Tree::TreeItem* identifier = procedureIdentifierChilds.at(0)->getChilds().at(0);
	procedureName = identifier->getStringData();
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

	if (blockChilds.at(1)->getData() != BEGIN) {
		std::cout << "ERROR! BEGIN expected, but got " << blockChilds.at(1)->getStringData() << std::endl;
		return;
	}
	endDataSegment();
	startCodeSegment();
	std::cout << procedureName << " PROC" << std::endl;
	assemblerProgram += procedureName + " PROC\n";
	caseStatementList(blockChilds.at(2));
	assemblerProgram += procedureName + " ENDP\n";
	std::cout << procedureName << " ENDP" << std::endl;
	endCodeSegment();
	std::cout << "END" << std::endl;
	assemblerProgram += "END\n";
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
	if (variableDeclarationsChild.at(0)->getData() != VAR) {
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

	Tree::TreeItem* variable = caseVariableIdentifier(declarationChild.at(0), true);
	if (variable == nullptr) return false;

	if (declarationChild.at(1)->getData() != COLON) {
		std::cout << "ERROR! Expected ':'(0), but got " << declarationChild.at(1)->getStringData() << std::endl;
		return false;
	}

	Attribute* attribute = caseAttribtue(declarationChild.at(2));
	if (attribute == nullptr) return false;

	Attribute* attributeList = caseAttributeList(declarationChild.at(3));
	if (attributeList == nullptr) return false;

	attribute = attributeList->getType() == Attribute::IdentifierType::EMPTY ? attribute : attributeList;

	if (isIdentifierDeclarated(variable->getData())) {
		std::cout << "ERROR! " << variable->getStringData() << " already declarated!" << std::endl;
		return false;
	}

	Identifier* identifier = new Identifier(variable->getStringData(), variable->getData(), attribute);
	addIdentifier(*identifier);
	generateAsm(identifier);

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
			assemblerProgram += "\tnop\n";
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
	if (statementChilds.at(0)->getData() == LOOP) {
		return caseStatementLoop(item);
	}
	
	//expression
	return caseStatementExpression(item);
}

bool Translator::caseStatementLoop(Tree::TreeItem* item) {
	int currentLoopCounter = loopLabelCounter;
	std::cout << "; LOOP" << std::endl;
	assemblerProgram += "\t\t; LOOP\n";
	assemblerProgram += "?L" + std::to_string(loopLabelCounter) + ": nop\n";
	std::cout << "?L" << loopLabelCounter << ": nop" << std::endl;
	loopLabelCounter++;

	std::vector<Tree::TreeItem*> statementLoopChilds = item->getChilds();

	bool isStatemntList = caseStatementList(statementLoopChilds.at(1));
	if (isStatemntList == false) return false;

	if (statementLoopChilds.at(2)->getData() != ENDLOOP) {
		std::cout << "ERROR (line " << statementLoopChilds.at(2)->getRow() << " column " << statementLoopChilds.at(2)->getColumn() << " ! ENDLOOP expected, but got " << statementLoopChilds.at(2)->getStringData() << std::endl;
		return false;
	}

	std::cout << "; ENDLOOP" << std::endl;
	assemblerProgram += "\t\t; ENDLOOP\n";
	assemblerProgram += "\tJMP ?L" + std::to_string(currentLoopCounter) + "\n\tnop\n";
	std::cout << "JMP ?L" << currentLoopCounter << std::endl;
	std::cout << "?L" << loopLabelCounter << std::endl << "nop" << std::endl;
	loopLabelCounter++;

	return true;
}

bool Translator::caseStatementExpression(Tree::TreeItem* item) {
	std::vector<Tree::TreeItem*> statementExpressionsChild = item->getChilds();

	Expression* variable = caseVariable(statementExpressionsChild.at(0), false);
	if (variable == nullptr) return false;

	if (statementExpressionsChild.at(1)->getData() != EQUALS) {
		std::cout << "ERROR (line " << statementExpressionsChild.at(1)->getRow() << " column " << statementExpressionsChild.at(1)->getColumn() << " !  := expected, but got " << statementExpressionsChild.at(1)->getStringData() << std::endl;
		return false;
	}

	//TODO: add [3] item must be ;
	Expression* expression = caseExpression(statementExpressionsChild.at(2), false);
	if (expression == nullptr) return false;
	if (expression->getDimension() == nullptr || variable->getDimension() == nullptr) {
		Identifier* left = getIdentifier(variable->getCode());
		Identifier* right = getIdentifier(expression->getCode());
		if (right != nullptr && left != nullptr) {
			if ((expression->getDimension() == nullptr && variable->getDimension() == nullptr && left->getAttribute()->getType() != right->getAttribute()->getType())
				|| (expression->getDimension() != nullptr && left->getAttribute()->getType() != Attribute::INTEGER)
				|| (variable->getDimension() != nullptr && right->getAttribute()->getType() != Attribute::INTEGER)) {
				std::cout << "ERROR(line " << variable->getLine() << " column " << variable->getColumn() << " ! " << left->getName() << " and " << right->getName() << " MUST have same type" << std::endl;
				return false;
			}
		} else if (right == nullptr) {
			// right is UNSIGNED_INTEGER
			if ((left->getAttribute()->getType() == Attribute::RANGE && variable->getDimension() == nullptr) && left->getAttribute()->getType() != Attribute::INTEGER) {
				std::cout << "ERROR(line " << variable->getLine() << " column " << variable->getColumn() << " ! " << left->getName() << " and " << expression->getName() << " MUST have same type" << std::endl;
				return false;
			}
		}
	}

	assemblerProgram += "\t\t; " + variable->getVariableForPrint() + " := " + expression->getVariableForPrint() + "\n";
	std::cout << "; ";
	variable->print();
	std::cout << " := ";
	expression->print();
	std::cout << std::endl;
	generateAsm(variable, expression);

	return true;
}

Expression* Translator::caseExpression(Tree::TreeItem* item, bool isDimension) {
	if (item->getRule() != EXPRESSION) {
		std::cout << "ERROR (line " << item->getRow() << " column " << item->getColumn() << " ! EXPRESSION expected, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> expressionChilds = item->getChilds();

	if (expressionChilds.size() != 1) {
		std::cout << "ERROR! <expression> must have 1 child, but got " << expressionChilds.size() << std::endl;
		return nullptr;
	}

	Tree::TreeItem* unsignedInteger = nullptr;
	switch (expressionChilds.at(0)->getRule()) {
	case UNSIGNED_INTEGER:
		unsignedInteger = caseUnsignedInteger(expressionChilds.at(0));
		if (unsignedInteger == nullptr) return nullptr;
		return new Expression(
			unsignedInteger->getStringData(), 
			unsignedInteger->getData(), 
			unsignedInteger->getRow(),
			unsignedInteger->getColumn(),
			Expression::ExpressionType::UNSIGNED_INTEGER
		);
	case VARIABLE: return caseVariable(expressionChilds.at(0), isDimension);
	default: 
		std::cout << "ERROR! UNSIGNED_INTEGER or VARIABLE expected, but got " << expressionChilds.at(0)->getRule() << std::endl;
		return nullptr;
	}
}

Expression* Translator::caseDimension(Tree::TreeItem* item) {
	if (item->getRule() != DIMENSION) {
		std::cout << "ERROR (line " << item->getRow() << " column " << item->getColumn() << " ! DIMENSION expected, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> dimensionChilds = item->getChilds();

	if (dimensionChilds.size() == 1) {
		if (dimensionChilds.at(0)->getRule() == EMPTY) {
			return new Expression(
				dimensionChilds.at(0)->getStringData(),
				dimensionChilds.at(0)->getData(),
				dimensionChilds.at(0)->getRow(),
				dimensionChilds.at(0)->getColumn(),
				Expression::ExpressionType::EMPTY
			);
		} else {
			std::cout << "ERROR! if size of <dimension> is 1 it must be EMPTY" << std::endl;
			return nullptr;
		}
	}

	if (dimensionChilds.size() != 3) {
		std::cout << "ERROR! <diemnsion> MUST have 1 or 3 childs, but got " << dimensionChilds.size() << std::endl;
		return nullptr;
	}
	
	if (dimensionChilds.at(0)->getData() != ReserverWords::LEFT_SQUARE_BRACKET) {
		std::cout << "ERROR! '[' expected, but got " << dimensionChilds.at(1)->getStringData() << std::endl;
		return nullptr;
	}

	if (dimensionChilds.at(2)->getData() != ReserverWords::RIGHT_SQUARE_BRACKET) {
		std::cout << "ERROR! ']' expected, but got " << dimensionChilds.at(3)->getStringData() << std::endl;
		return nullptr;
	}

	if (dimensionChilds.at(1)->getRule() != Rules::EXPRESSION) {
		std::cout << "ERROR! 2-nd child of <dimension> must be EXPRESSION, but got " << dimensionChilds.at(1)->getRule() << std::endl;
		return nullptr;
	}

	return caseExpression(dimensionChilds.at(1), true);
}

Expression* Translator::caseVariable(Tree::TreeItem* item, bool onlyIntegerValues) {
	if (item->getRule() != VARIABLE) {
		std::cout << "ERROR (line " << item->getRow() << " column " << item->getColumn() << " ! VARIABLE expected, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> variableChilds = item->getChilds();

	if (variableChilds.size() < 2) {
		std::cout << "ERROR! <variable> MUST have at least 2 childs, but got " << variableChilds.size();
		return nullptr;
	}

	Tree::TreeItem* variableIdentifier = caseVariableIdentifier(variableChilds.at(0), false);
	if (variableIdentifier == nullptr) return nullptr;

	Expression* dimension = caseDimension(variableChilds.at(1));
	if (dimension == nullptr) return nullptr;
	if (dimension->getType() != Expression::ExpressionType::EMPTY && !isIdentifierIsRange(variableIdentifier->getData())) {
		std::cout << "ERROR! operator [..] can use only for RANGE type, but " << variableIdentifier->getStringData() << " is not type of range." << std::endl;
		return nullptr;
	}
	
	if (dimension->getType() == Expression::ExpressionType::EMPTY) {
		if (onlyIntegerValues && !isIdentifierIsInteger(variableIdentifier->getData())) {
			std::cout << "ERROR! INSIDE [ ] can be only INTEGER VARIABLE, but got " << variableIdentifier->getStringData() << std::endl;
			return nullptr;
		}
		return new Expression(
			variableIdentifier->getStringData(),
			variableIdentifier->getData(),
			variableIdentifier->getRow(),
			variableIdentifier->getColumn(),
			Expression::ExpressionType::VARIABLE
		);
	}

	return new Expression(
		variableIdentifier->getStringData(),
		variableIdentifier->getData(),
		variableIdentifier->getRow(),
		variableIdentifier->getColumn(),
		Expression::ExpressionType::VARIABLE,
		dimension
	);
}

Tree::TreeItem* Translator::caseVariableIdentifier(Tree::TreeItem* item, bool isIgnoreCreationCheck) {
	if (item->getRule() != VARIABLE_IDENTIFIER) {
		std::cout << "ERROR (line " << item->getRow() << " column " << item->getColumn() << " ! Expected VARIABLE_IDENTIFIER, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> variableIdentifierChilds = item->getChilds();
	if (variableIdentifierChilds.size() != 1) {
		std::cout << "ERROR! <variable-identifier> MUST have only 1 child, but got" << variableIdentifierChilds.size() << std::endl;
		return nullptr;
	}

	return caseIdentifier(variableIdentifierChilds.at(0), isIgnoreCreationCheck);
}

Tree::TreeItem* Translator::caseIdentifier(Tree::TreeItem* item, bool ignoreCreationCheck) {
	if (item->getRule() != IDENTIFIER_RULE) {
		std::cout << "ERROR (line " << item->getRow() << " column " << item->getColumn() << " ! Expected IDENTIFIER_RULE, but got " << item->getRule() << std::endl;
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

	if (!ignoreCreationCheck && !isIdentifierDeclarated(identifier->getData())) {
		std::cout << "ERROR! Identifier " << identifier->getStringData() << " IS NOT DECLARATED!" << std::endl;
		return nullptr;
	}

	return identifier;
}

Attribute* Translator::caseAttribtue(Tree::TreeItem* item) {
	if (item->getRule() != ATTRIBUTE) {
		std::cout << "ERROR (line " << item->getRow() << " column " << item->getColumn() << " ! ATTRIBUTE expected, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> attributeChilds = item->getChilds();

	if (attributeChilds.size() == 1) {
		if (attributeChilds.at(0)->getData() == FLOAT) {
			return new Attribute(Attribute::IdentifierType::FLOAT);
		} else if (attributeChilds.at(0)->getData() == INTEGER) {
			return new Attribute(Attribute::IdentifierType::INTEGER);
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

Attribute* Translator::caseAttributeList(Tree::TreeItem* item) {
	if (item->getRule() != ATTRIBUTES_LIST) {
		std::cout << "ERROR (line " << item->getRow() << " column " << item->getColumn() << " ! <attribute-list> expected, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> attributeListChilds = item->getChilds();

	if (attributeListChilds.size() == 1) {
		if (attributeListChilds.at(0)->getRule() == EMPTY) {
			return new Attribute(Attribute::IdentifierType::EMPTY);
		} else {
			std::cout << "ERROR! <attribute-list> with childs count 1 MUST have only EMPTY child" << std::endl;
			return nullptr;
		}
	}

	if (attributeListChilds.size() != 2) {
		std::cout << "ERROR! <attribute-list> MUST contains 2 childs" << std::endl;
		return nullptr;
	}

	Attribute* attribute = caseAttribtue(attributeListChilds.at(0));
	if (attribute == nullptr) return nullptr;

	Attribute* attributeList = caseAttributeList(attributeListChilds.at(1));

	return attributeList->getType() == Attribute::IdentifierType::EMPTY ? attribute : attributeList;
}

RangeAttribute* Translator::caseRange(Tree::TreeItem* item) {
	if (item->getRule() != RANGE) {
		std::cout << "ERROR (line " << item->getRow() << " column " << item->getColumn() << " ! Eexpected RANGE, but got " << item->getRule() << std::endl;
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
	if (divider->getData() != DOUBLE_DOT) {
		std::cout << "ERROR(line " << divider->getRow() << " column " << divider->getColumn() << " ! Expected .., but got " << divider->getStringData() << std::endl;
		return nullptr;
	}

	Tree::TreeItem* to = caseUnsignedInteger(rangeChilds.at(2));
	if (to == nullptr) return nullptr;

	std::string str = from->getStringData() + " " + divider->getStringData() + " " + to->getStringData();
	return new RangeAttribute(std::stoi(from->getStringData()), std::stoi(to->getStringData()));
}

Tree::TreeItem* Translator::caseUnsignedInteger(Tree::TreeItem* item) {
	if (item->getRule() != UNSIGNED_INTEGER) {
		std::cout << "ERROR (line " << item->getRow() << " column " << item->getColumn() << " ! Expected UNSIGNED_INTEGER, but got " << item->getRule() << std::endl;
		return nullptr;
	}

	std::vector<Tree::TreeItem*> unsignedIntegerChilds = item->getChilds();

	if (unsignedIntegerChilds.size() != 1) {
		std::cout << "ERROR! <unsigned-integer> MUST have 1 child, but got " << unsignedIntegerChilds.size() << std::endl;
		return nullptr;
	}

	Tree::TreeItem* integer = unsignedIntegerChilds.at(0);
	if (integer->getRule() != ADDING_CONSTANT) {
		std::cout << "ERROR (line " << integer->getRow() << " column " << integer->getColumn() << " ! Expected CONSTANT, but got " << integer->getRule() << std::endl;
		return nullptr;
	}

	return integer;
}

void Translator::generateAsm(Identifier* identifier) {
	switch (identifier->getType()) {
	case Attribute::IdentifierType::RANGE:
		std::cout << identifier->getName() << " dw ";
		assemblerProgram += "\t" + identifier->getName() + " dw ";
		if (RangeAttribute* attribute = dynamic_cast<RangeAttribute*>(identifier->getAttribute())) {
			int start = attribute->getFrom() > attribute->getTo() ? attribute->getTo() : attribute->getFrom();
			int end = attribute->getFrom() < attribute->getTo() ? attribute->getTo() : attribute->getFrom();
			for (int from = start; from <= end; from++) {
				assemblerProgram += std::to_string(from);
				std::cout << from;
				if (from != attribute->getTo()) {
					std::cout << ", ";
					assemblerProgram += ", ";
				}
			}
			assemblerProgram += "\n";
			std::cout << std::endl;
		}
		break;
	case Attribute::IdentifierType::FLOAT:
		std::cout << identifier->getName() << " dw ?" << std::endl;
		assemblerProgram += "\t" + identifier->getName() + " dw ?\n";
		break;
	case Attribute::IdentifierType::INTEGER:
		std::cout << identifier->getName() << " dw ?" << std::endl;
		assemblerProgram += "\t" + identifier->getName() + " dw ?\n";
		break;
	}
}

void Translator::generateAsm(Expression* left, Expression* right) {
	std::string leftStr = generatePrepareLeftExpression(left, true);
	std::string rightStr = generatePrepareRightExpression(right, true);
	std::cout << leftStr << ", " << rightStr << std::endl;
	assemblerProgram += leftStr + ", " + rightStr + "\n";
}

std::string Translator::generatePrepareLeftExpression(Expression* expression, bool isLastStatement) {
	if (expression->getDimension() == nullptr && isLastStatement) {
		return "\tMOV " + expression->getName();
	} else if (expression->getDimension() == nullptr) {
		return "\tMOV BX, " + expression->getName() + "\n";
	}
	
	std::string dimension = generatePrepareLeftExpression(expression->getDimension(), false);
	std::cout << dimension;
	assemblerProgram += dimension;

	if (isLastStatement) {
		return "\tMOV " + expression->getName() + "[BX]";
	} else {
		return "\tMOV BX, " + expression->getName() + "[BX]\n";
	}

}

std::string Translator::generatePrepareRightExpression(Expression* expression, bool isLastStatement) {
	if (expression->getDimension() == nullptr && isLastStatement) {
		std::cout << "MOV BP, " + expression->getName() << std::endl;
		assemblerProgram += "\tMOV BP, " + expression->getName() + "\n";
		return "BP";
	} else if (expression->getDimension() == nullptr) {
		std::cout << "MOV BP, " << expression->getName() << std::endl;
		assemblerProgram += "\tMOV BP, " + expression->getName() + "\n";
		return "BP";
	}

	generatePrepareRightExpression(expression->getDimension(), false);
	std::cout << "MOV BP, " << expression->getName() + "[BP]" << std::endl;
	assemblerProgram += "\tMOV BP, " + expression->getName() + "[BP]\n";
	return "BP";
}

void Translator::dumpIntoFile(std::string filePath) {
	std::ofstream file;
	file.open(filePath);
	file << assemblerProgram << std::endl;
	file.close();
}

void Translator::startDataSegment(){
	std::cout << "DATA SEGMENT" << std::endl;
	assemblerProgram += "DATA SEGMENT\n";
}

void Translator::endDataSegment(){
	std::cout << "DATA ENDS" << std::endl;
	assemblerProgram += "DATA ENDS\n";
}

void Translator::startCodeSegment(){
	std::cout << "CODE SEGMENT" << std::endl;
	std::cout << "ASSUME CS:CODE, DS:DATA" << std::endl;
	assemblerProgram += "CODE SEGMENT\n";
	assemblerProgram += "ASSUME CS:CODE, DS:DATA\n";
}

void Translator::endCodeSegment(){
	std::cout << "CODE ENDS" << std::endl;
	assemblerProgram += "CODE ENDS\n";
}
