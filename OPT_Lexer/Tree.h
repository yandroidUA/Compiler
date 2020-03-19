#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

class Tree {
public:

	Tree();
	~Tree() = default;

	struct TreeItem {
	public:
		
		TreeItem(std::string& data) {
			this->data = data;
		}

		~TreeItem() = default;

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

			std::cout << data << std::endl;
			
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

			fileStream << data << std::endl;

			for (auto& child : childs) {
				child->dumpIntoFile(fileStream, tabulation + 2);
			}
		}

	private:
		std::vector<TreeItem*> childs;
		std::string data;
	};

	/*	add child to lastModified
		DON'T modify the lastModified
		return pointer to added item
	*/
	TreeItem* addChild(std::string&);
	TreeItem* addChild(const char*);
	TreeItem* addChild(int);

	/*	add next to lastModified 
		lastModified become added TreeItem 
		return pointer to added item
	*/
	TreeItem* addNext(std::string&);
	TreeItem* addNext(const char*);
	TreeItem* addNext(int);

	TreeItem* getCurrent();

	/*	lastModifiend = param
		TRUE  -> if switch is successfull
		FALSE -> if not
	*/
	bool switchTo(TreeItem*);

	void print();
	void dumpIntoFile(std::ofstream&);

private:
	TreeItem* root;
	TreeItem* lastModified;
	int depth;
	int elements;

	TreeItem* add(std::string&, bool);
};

