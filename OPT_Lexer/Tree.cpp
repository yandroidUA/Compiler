#include "Tree.h"

Tree::Tree() {
	this->root = nullptr;
	this->lastModified = nullptr;
	this->elements = 0;
	this->depth = 0;
}

Tree::TreeItem* Tree::addChild(std::string data, int row, int column, Rules rules, int value) {
	return add(data, row, column, rules, value, false);
}

Tree::TreeItem* Tree::addChild(const char* data, int row, int column, Rules rules, int value) {
	std::string str = data;
	return add(str, row, column, rules, value, false);
}

Tree::TreeItem* Tree::addChild(int data, int row, int column, Rules rules, int value) {
	std::string str = std::to_string(data);
	return add(str, row, column, rules, value, false);
}

Tree::TreeItem* Tree::addNext(std::string data, int row, int column, Rules rules, int value) {
	depth++;
	return add(data, row, column, rules, value, true);
}

Tree::TreeItem* Tree::addNext(const char* data, int row, int column, Rules rules, int value) {
	std::string str = data;
	return add(str, row, column, rules, value, true);
}

Tree::TreeItem* Tree::addNext(int data, int row, int column, Rules rules, int value) {
	std::string str = std::to_string(data);
	return add(str, row, column, rules, value, true);
}

Tree::TreeItem* Tree::add(std::string& data, int row, int column, Rules rules, int value, bool isModify) {
	elements++;
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
