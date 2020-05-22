#include "Translator.h"
#include "ReservedWords.h"
#include "Identifier.h"

std::string dataSegmentString = "";
std::string codeSegmentString = "";

Translator::Translator(Tree* tree) {
	this->tree = tree;
	this->errorHappened = false;
	this->errorString = "";
	this->assemblerProgram = "";
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

void Translator::handleError(std::string reason) {
	std::cout << reason << std::endl;
	errorString += reason + "\n";
	errorHappened = true;
}

void Translator::handleError(std::string reason, Tree::TreeItem* item){
	std::cout << reason << std::endl;
	errorString += reason + ", but got " + item->getStringData();
	if (item->getRow() != -1) {
		errorString += " (row: " + std::to_string(item->getRow());
	}
	if (item->getColumn() != -1) {
		errorString += ", column: " + std::to_string(item->getColumn()) + ")";
	}
	errorString += "\n";
	errorHappened = true;
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

bool Translator::caseProgram(Tree::TreeItem* item) {
	std::vector<Tree::TreeItem*> programChilds = item->getChilds();
	if (programChilds.size() != 2) {
		handleError("Error! Program must have <procedure-identifier> AND <block>");
		return false;
	}

	return caseProcedureIdentifier(programChilds.at(0)) == false ? false : caseBlock(programChilds.at(1));
}

bool Translator::caseProcedureIdentifier(Tree::TreeItem* item) {
	std::vector<Tree::TreeItem*> procedureIdentifierChilds = item->getChilds();
	if (procedureIdentifierChilds.size() != 1 || procedureIdentifierChilds.at(0)->getRule() != IDENTIFIER_RULE) {
		handleError("Error! <procedure-identifier> must contains IDENTIFIER");
		return false;
	}

	Tree::TreeItem* identifier = procedureIdentifierChilds.at(0)->getChilds().at(0);
	procedureName = identifier->getStringData();
	return true;
}

bool Translator::caseBlock(Tree::TreeItem* item) {
	std::vector<Tree::TreeItem*> blockChilds = item->getChilds();

	if (blockChilds.size() != 4) {
		handleError("ERROR! <block> MUST have 4 childs: <variable-declarations>, BEGIN, <statement-list>, END, but got " + std::to_string(blockChilds.size()));
		return false;
	}

	if (!caseVariableDeclarations(blockChilds.at(0))) {
		return false;
	}

	if (blockChilds.at(1)->getData() != BEGIN) {
		handleError("ERROR! BEGIN expected", blockChilds.at(1));
		return false;
	}
	endDataSegment();
	startCodeSegment();
	assemblerProgram += procedureName + " PROC\n";
	if (caseStatementList(blockChilds.at(2)) == false) {
		return false;
	}
	assemblerProgram += procedureName + " ENDP\n";
	endCodeSegment();
	assemblerProgram += "END\n";
	return true;
}

bool Translator::caseVariableDeclarations(Tree::TreeItem* item) {
	if (item->getRule() != VARIABLE_DECLARATIONS) {
		handleError("ERROR! <variable-declarations> expected", item);
		return false;
	}

	std::vector<Tree::TreeItem*> variableDeclarationsChild = item->getChilds();

	if (variableDeclarationsChild.size() == 1) {
		if (variableDeclarationsChild.at(0)->getRule() != Rules::EMPTY) {
			handleError("ERROR! <empty> expected", variableDeclarationsChild.at(0));
			return false;
		}
		startDataSegment();
		return true;
	}

	if (variableDeclarationsChild.size() != 2) {
		handleError("ERROR! <variable-declarations> must have only 2 childs or <empty>, but got " + std::to_string(variableDeclarationsChild.size()));
		return false;
	}

	if (variableDeclarationsChild.at(0)->getData() != VAR) {
		handleError("VAR(404) expected", variableDeclarationsChild.at(0));
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
		handleError("ERROR! DECLARATION expected", item);
		return false;
	}

	std::vector<Tree::TreeItem*> declarationChild = item->getChilds();

	if (declarationChild.size() < 4) {
		handleError("ERROR! DECLARATIONS_LIST MUST have at least 4 childs, but got " + std::to_string(declarationChild.size()));
		return false;
	}

	Tree::TreeItem* variable = caseVariableIdentifier(declarationChild.at(0), true);
	if (variable == nullptr) return false;

	if (declarationChild.at(1)->getData() != COLON) {
		handleError("ERROR! Expected ':'(0)", declarationChild.at(1));
		return false;
	}

	Attribute* attribute = caseAttribtue(declarationChild.at(2));
	if (attribute == nullptr) return false;

	Attribute* attributeList = caseAttributeList(declarationChild.at(3));
	if (attributeList == nullptr) return false;

	attribute = attributeList->getType() == Attribute::IdentifierType::EMPTY ? attribute : attributeList;

	if (isIdentifierDeclarated(variable->getData())) {
		handleError("ERROR! " + variable->getStringData() + " already declarated! (row: " + std::to_string(item->getRow()) + ", column: " + std::to_string(item->getColumn()) + ")");
		return false;
	}

	Identifier* identifier = new Identifier(variable->getStringData(), variable->getData(), attribute);
	addIdentifier(*identifier);
	generateAsm(identifier);

	return true; 
}

bool Translator::caseDeclarationList(Tree::TreeItem* item) {
	if (item->getRule() != DECLARATIONS_LIST) {
		handleError("ERROR! Expected DECLARATIONS_LIST, but got " + item->getStringData());
		return false;
	}

	std::vector<Tree::TreeItem*> declarationListChilds = item->getChilds();

	if (declarationListChilds.size() == 1) {
		if (declarationListChilds.at(0)->getRule() == EMPTY) {
			return true;
		} else {
			handleError("ERROR! <declaration-list> with 1 child MUST have EMPTY child");
			return false;
		}
	}

	if (declarationListChilds.size() == 2) {
		bool isDeclaration = caseDeclaration(declarationListChilds.at(0));
		return isDeclaration ? caseDeclarationList(declarationListChilds.at(1)) : isDeclaration;
	} else {
		handleError("ERROR! <declaration-list> must have 1 or 2 childs, but got " + std::to_string(declarationListChilds.size()));
		return false;
	}
}

bool Translator::caseStatementList(Tree::TreeItem* item) {
	if (item->getRule() != STATEMENT_LIST) {
		handleError("ERROR! Expected <statement-list>", item);
		return false;
	}

	std::vector<Tree::TreeItem*> statementListChild = item->getChilds();

	if (statementListChild.size() == 1) {
		if (statementListChild.at(0)->getRule() == EMPTY) {
			std::cout << "nop" << std::endl;
			assemblerProgram += "\tnop\n";
			return true;
		} else {
			handleError("ERROR! If <statement-list> has 1 child it MUST be EMPTY", statementListChild.at(0));
			return false;
		}
	}

	if (statementListChild.size() != 2) {
		handleError("ERROR! <statement-list> MUST have 1 or 2 childs, but got " + std::to_string(statementListChild.size()));
		return false;
	}
	
	bool isStatement = caseStatement(statementListChild.at(0));

	return isStatement ? caseStatementList(statementListChild.at(1)) : isStatement;
}

bool Translator::caseStatement(Tree::TreeItem* item) {
	if (item->getRule() != STATEMENT) {
		handleError("ERROR! STATEMENT expected", item);
		return false;
	}

	std::vector<Tree::TreeItem*> statementChilds = item->getChilds();

	if (statementChilds.size() != 4) {
		handleError("ERROR! <statement-list> MUST have 4 child items, but got " + std::to_string(statementChilds.size()));
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
		handleError("ERROR! ENDLOOP expected", statementLoopChilds.at(2));
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
		handleError("ERROR! ':=' expected", statementExpressionsChild.at(1));
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
				handleError("ERROR! " + left->getName() + " and " + right->getName() + " MUST have same type (row: " + std::to_string(variable->getLine()) + ", column: " + std::to_string(variable->getColumn()) + ")");
				return false;
			}
		} else if (right == nullptr) {
			// right is UNSIGNED_INTEGER
			if ((left->getAttribute()->getType() == Attribute::RANGE && variable->getDimension() == nullptr) && left->getAttribute()->getType() != Attribute::INTEGER) {
				handleError("ERROR! " + left->getName() + " and " + expression->getName() + " MUST have same type (row: " + std::to_string( variable->getLine()) + ", column: " + std::to_string(variable->getColumn()) + ")");
				return false;
			}
		}
	}

	assemblerProgram += "\t\t; " + variable->getVariableForPrint() + " := " + expression->getVariableForPrint() + "\n";
	variable->print();
	expression->print();
	generateAsm(variable, expression);

	return true;
}

Expression* Translator::caseExpression(Tree::TreeItem* item, bool isDimension) {
	if (item->getRule() != EXPRESSION) {
		handleError("ERROR! EXPRESSION expected", item);
		return nullptr;
	}

	std::vector<Tree::TreeItem*> expressionChilds = item->getChilds();

	if (expressionChilds.size() != 1) {
		handleError("ERROR! <expression> must have 1 child, but got " + std::to_string(expressionChilds.size()));
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
		handleError("ERROR! UNSIGNED_INTEGER or VARIABLE expected", expressionChilds.at(0));
		return nullptr;
	}
}

Expression* Translator::caseDimension(Tree::TreeItem* item) {
	if (item->getRule() != DIMENSION) {
		handleError("ERROR! DIMENSION expected", item);
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
			handleError("ERROR! if size of <dimension> is 1 it must be EMPTY", item);
			return nullptr;
		}
	}

	if (dimensionChilds.size() != 3) {
		handleError("ERROR! <diemnsion> MUST have 1 or 3 childs, but got " + std::to_string(dimensionChilds.size()));
		return nullptr;
	}
	
	if (dimensionChilds.at(0)->getData() != ReserverWords::LEFT_SQUARE_BRACKET) {
		handleError("ERROR! '[' expected", dimensionChilds.at(1));
		return nullptr;
	}

	if (dimensionChilds.at(2)->getData() != ReserverWords::RIGHT_SQUARE_BRACKET) {
		handleError("ERROR! ']' expected, but got ", dimensionChilds.at(3));
		return nullptr;
	}

	if (dimensionChilds.at(1)->getRule() != Rules::EXPRESSION) {
		handleError("ERROR! 2-nd child of <dimension> must be EXPRESSION, but got ", dimensionChilds.at(1));
		return nullptr;
	}

	return caseExpression(dimensionChilds.at(1), true);
}

Expression* Translator::caseVariable(Tree::TreeItem* item, bool onlyIntegerValues) {
	if (item->getRule() != VARIABLE) {
		handleError("ERROR! VARIABLE expected", item);
		return nullptr;
	}

	std::vector<Tree::TreeItem*> variableChilds = item->getChilds();

	if (variableChilds.size() < 2) {
		handleError("ERROR! <variable> MUST have at least 2 childs, but got " + std::to_string(variableChilds.size()));
		return nullptr;
	}

	Tree::TreeItem* variableIdentifier = caseVariableIdentifier(variableChilds.at(0), false);
	if (variableIdentifier == nullptr) return nullptr;

	Expression* dimension = caseDimension(variableChilds.at(1));
	if (dimension == nullptr) return nullptr;
	if (dimension->getType() != Expression::ExpressionType::EMPTY && !isIdentifierIsRange(variableIdentifier->getData())) {
		handleError("ERROR! operator [..] can use only for RANGE type", variableIdentifier);
		return nullptr;
	}
	
	if (dimension->getType() == Expression::ExpressionType::EMPTY) {
		if (onlyIntegerValues && !isIdentifierIsInteger(variableIdentifier->getData())) {
			handleError("ERROR! INSIDE [ ] can be only INTEGER VARIABLE", variableIdentifier);
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
		handleError("ERROR! Expected VARIABLE_IDENTIFIER", item);
		return nullptr;
	}

	std::vector<Tree::TreeItem*> variableIdentifierChilds = item->getChilds();
	if (variableIdentifierChilds.size() != 1) {
		handleError("ERROR! <variable-identifier> MUST have only 1 child, but got" + std::to_string(variableIdentifierChilds.size()));
		return nullptr;
	}

	return caseIdentifier(variableIdentifierChilds.at(0), isIgnoreCreationCheck);
}

Tree::TreeItem* Translator::caseIdentifier(Tree::TreeItem* item, bool ignoreCreationCheck) {
	if (item->getRule() != IDENTIFIER_RULE) {
		handleError("ERROR! Expected IDENTIFIER_RULE", item);
		return nullptr;
	}

	std::vector<Tree::TreeItem*> identifierRuleChilds = item->getChilds();

	if (identifierRuleChilds.size() != 1) {
		handleError("ERROR! IDENTIFIER_RULE MUST have only 1 child, but got " + std::to_string(identifierRuleChilds.size()));
		return nullptr;
	}

	Tree::TreeItem* identifier = identifierRuleChilds.at(0);
	if (identifier->getRule() != ADDING_INDENTIFIER) {
		handleError("ERROR! Expected ADDING_INDENTIFIE", identifier);
		return nullptr;
	}

	if (!ignoreCreationCheck && !isIdentifierDeclarated(identifier->getData())) {
		handleError("ERROR! Identifier " + identifier->getStringData() + " IS NOT DECLARATED! (line: " + std::to_string(identifier->getRow()) + ", column: " + std::to_string(identifier->getColumn()) + ")");
		return nullptr;
	}

	return identifier;
}

Attribute* Translator::caseAttribtue(Tree::TreeItem* item) {
	if (item->getRule() != ATTRIBUTE) {
		handleError("ERROR! Expected ATTRIBUTE", item);
		return nullptr;
	}

	std::vector<Tree::TreeItem*> attributeChilds = item->getChilds();

	if (attributeChilds.size() == 1) {
		if (attributeChilds.at(0)->getData() == FLOAT) {
			return new Attribute(Attribute::IdentifierType::FLOAT);
		} else if (attributeChilds.at(0)->getData() == INTEGER) {
			return new Attribute(Attribute::IdentifierType::INTEGER);
		} else {
			handleError("ERROR! INTEGER or FLOAT expected", attributeChilds.at(0));
			return nullptr;
		}
	}

	if (attributeChilds.size() != 3) {
		handleError("ERROR! <attribute> MUST have 1 or 3 childs, but got " + std::to_string(attributeChilds.size()));
		return nullptr;
	}
	
	return caseRange(attributeChilds.at(1));
}

Attribute* Translator::caseAttributeList(Tree::TreeItem* item) {
	if (item->getRule() != ATTRIBUTES_LIST) {
		handleError("ERROR! <attribute-list> expected", item);
		return nullptr;
	}

	std::vector<Tree::TreeItem*> attributeListChilds = item->getChilds();

	if (attributeListChilds.size() == 1) {
		if (attributeListChilds.at(0)->getRule() == EMPTY) {
			return new Attribute(Attribute::IdentifierType::EMPTY);
		} else {
			handleError("ERROR! <attribute-list> with childs count 1 MUST have only EMPTY child, but got " + std::to_string(attributeListChilds.size()));
			return nullptr;
		}
	}

	if (attributeListChilds.size() != 2) {
		handleError("ERROR!  <attribute-list> MUST contains 2 childs, but got " + std::to_string(attributeListChilds.size()));
		return nullptr;
	}

	Attribute* attribute = caseAttribtue(attributeListChilds.at(0));
	if (attribute == nullptr) return nullptr;

	Attribute* attributeList = caseAttributeList(attributeListChilds.at(1));

	return attributeList->getType() == Attribute::IdentifierType::EMPTY ? attribute : attributeList;
}

RangeAttribute* Translator::caseRange(Tree::TreeItem* item) {
	if (item->getRule() != RANGE) {
		handleError("ERROR! RANGE expected", item);
		return nullptr;
	}

	std::vector<Tree::TreeItem*> rangeChilds = item->getChilds();
	if (rangeChilds.size() != 3) {
		handleError("ERROR!  <range> MUST have 3 childs, but got " + std::to_string(rangeChilds.size()));
		return nullptr;
	}

	Tree::TreeItem* from = caseUnsignedInteger(rangeChilds.at(0));
	if (from == nullptr) return nullptr;

	Tree::TreeItem* divider = rangeChilds.at(1);
	if (divider->getData() != DOUBLE_DOT) {
		handleError("ERROR! Expexted '..'", divider);
		return nullptr;
	}

	Tree::TreeItem* to = caseUnsignedInteger(rangeChilds.at(2));
	if (to == nullptr) return nullptr;

	std::string str = from->getStringData() + " " + divider->getStringData() + " " + to->getStringData();
	return new RangeAttribute(std::stoi(from->getStringData()), std::stoi(to->getStringData()));
}

Tree::TreeItem* Translator::caseUnsignedInteger(Tree::TreeItem* item) {
	if (item->getRule() != UNSIGNED_INTEGER) {
		handleError("ERROR! Expected UNSIGNED_INTEGER", item);
		return nullptr;
	}

	std::vector<Tree::TreeItem*> unsignedIntegerChilds = item->getChilds();

	if (unsignedIntegerChilds.size() != 1) {
		handleError("ERROR! <unsigned-integer> MUST have 1 child, but got " + std::to_string(unsignedIntegerChilds.size()));
		return nullptr;
	}

	Tree::TreeItem* integer = unsignedIntegerChilds.at(0);
	if (integer->getRule() != ADDING_CONSTANT) {
		handleError("Expected CONSTANT", integer);
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
	if (errorHappened) {
		file << errorString << std::endl;
	} else {
		file << assemblerProgram << std::endl;
	}
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
