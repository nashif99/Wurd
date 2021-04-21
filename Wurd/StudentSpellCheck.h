#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

using namespace std;

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() { }
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct Trie {
		vector<Trie*> child;
		bool endstring;
		char c;
	};
	struct Trie* createNode() { //create new node with null children
		struct Trie* tNode = new Trie;
		tNode->endstring = false;
		for (int i = 0; i < 28; i++)
			tNode->child.push_back(NULL);
		return tNode;
	}

	void insert(struct Trie* root, string key) {
		struct Trie* curr = root;
		for (int i = 0; i < key.length(); i++) { //iterate through the child nodes until valid position is found to insert
			key[i] = std::tolower(key[i]);
			int index = key[i] - 'a';
			if (key[i] == '\'') //for the case when the character is an apostrophe
				index = 27;
			if (!curr->child[index]) {
				curr->child[index] = createNode();
				
			}
			curr = curr->child[index];

		}
		curr->endstring= true;
	}

	bool search(struct Trie* root, string key) { 
		struct Trie* curr = root;
		for (int i = 0; i < key.length(); i++) { //iterate through the nodes to see if there is a match
			key[i] = std::tolower(key[i]);
			int index = key[i] - 'a';
			if (key[i] == '\'')
				index = 27;
			if (!curr->child[index])
				return false;
			curr = curr->child[index];
		}
		return (curr != NULL && curr->endstring);
	}

	void FreeTrie(Trie* root) {
		if (root == NULL)
			return;
		for (int i = 0; i < 28; i++) {
			FreeTrie(root->child[i]);
		}
		delete root;
	}
	struct Trie* root = createNode();
};

#endif  // STUDENTSPELLCHECK_H_
