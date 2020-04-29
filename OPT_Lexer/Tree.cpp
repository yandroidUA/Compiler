#include "Tree.h"

Tree::Tree() {
	this->root = nullptr;
	this->lastModified = nullptr;
}

Tree::TreeItem* Tree::addChild(std::string data, int row, int column, Rules rules, int value) {
	return add(data, row, column, rules, value, false);
}

Tree::TreeItem* Tree::addChild(LexerResult* dat, Rules rul) {
	std::string token = dat->getToken();
	return add(token, dat->getRowNumber(), dat->getRowNumber(), rul, dat->getCode(), false);
}

Tree::TreeItem* Tree::addNext(std::string data, int row, int column, Rules rules, int value) {
	return add(data, row, column, rules, value, true);
}

Tree::TreeItem* Tree::addNext(LexerResult* lexerResult, Rules rule) {
	std::string token = lexerResult->getToken();
	return add(token, lexerResult->getRowNumber(), lexerResult->getColumnNumber(), rule, lexerResult->getCode(), true);
}

Tree::TreeItem* Tree::add(std::string& data, int row, int column, Rules rules, int value, bool isModify) {
	TreeItem* item = new TreeItem(data, row, column, rules, value);
	if (root == nullptr) {
		root = item;
		lastModified = root;
		return lastModified;
	}

	item = lastModified->add(item);
	lastModified = isModify ? item : lastModified;
	return item;
}

bool Tree::switchTo(TreeItem* item) {
	if (item == nullptr) {
		return false;
	}

	lastModified = item;
	return true;
}

Tree::TreeItem* Tree::getCurrent() {
	return lastModified;
}

void Tree::dumpIntoFile(std::ofstream& outputStream) {
	if (root == nullptr) return;
	root->dumpIntoFile(outputStream, 0);
}

Tree::TreeItem* Tree::getRoot(){
	return root;
}

void Tree::print() {
	if (root == nullptr) return;
	root->print(0);
}
