#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "Rules.h"

class Tree {
public:

	Tree();
	~Tree() = default;

	struct TreeItem {
	public:

		TreeItem(std::string& data, int row, int column, Rules rule, int value) {
			this->data = data;
			this->value = value;
			this->rule = rule;
			this->row = row;
			this->column = column;
		}

		~TreeItem() = default;

		inline std::vector<TreeItem*> getChilds() {
			return childs;
		}

		inline Rules getRule() const {
			return rule;
		}

		inline std::string getStringData() const {
			return data;
		}

		inline int getData() const {
			return value;
		}

		inline int getRow() const {
			return row;
		}

		inline int getColumn() const {
			return column;
		}

		TreeItem* add(TreeItem* item) {
			childs.push_back(item);
			return item;
		}

		void print(int tabulation) {
			std::cout << "(" << tabulation << ")";
			int tab = tabulation;

			while (tab > 0) {
				std::cout << "*";
				tab--;
			}

			std::cout << data;
			if (value != -1) {
			std::cout << " " << value;
			}
			std::cout << std::endl;

			for (auto& child : childs) {
				child->print(tabulation + 2);
			}

		}

		void dumpIntoFile(std::ofstream& fileStream, int tabulation) {
			fileStream << "(" << tabulation << ")";
			int tab = tabulation;

			while (tab > 0) {
				fileStream << "*";
				tab--;
			}

			fileStream << data;
			if (value != -1) {
				fileStream << " " << value;
			}
			fileStream << std::endl;

			for (auto& child : childs) {
				child->dumpIntoFile(fileStream, tabulation + 2);
			}
		}

	private:
		// contains child of current rule
		std::vector<TreeItem*> childs;
		// contains rule as string (for output)
		std::string data;
		// contains id of rule
		Rules rule;
		// contains id of main resource (such as id of variable or identifier)
		int value;
		// row position
		int row;
		// column position
		int column;
	};

	/*	add child to lastModified
		DON'T modify the lastModified
		return pointer to added item
	*/
	TreeItem* addChild(std::string, int, int, Rules, int);
	TreeItem* addChild(const char*, int, int, Rules, int);
	TreeItem* addChild(int, int, int, Rules, int);

	/*	add next to lastModified 
		lastModified become added TreeItem 
		return pointer to added item
	*/
	TreeItem* addNext(std::string, int, int, Rules, int);
	TreeItem* addNext(const char*, int, int, Rules, int);
	TreeItem* addNext(int, int, int, Rules, int);

	TreeItem* getCurrent();

	/*	lastModifiend = param
		TRUE  -> if switch is successfull
		FALSE -> if not
	*/
	bool switchTo(TreeItem*);

	void print();
	void dumpIntoFile(std::ofstream&);
	TreeItem* getRoot();

private:
	TreeItem* root;
	TreeItem* lastModified;
	int depth;
	int elements;

	TreeItem* add(std::string&, int, int, Rules, int, bool);
};

