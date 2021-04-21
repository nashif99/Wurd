#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include<iostream>
#include<fstream>
SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
	// TODO

}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	
	ifstream infile(dictionaryFile);
	if (!infile) {
		//cerr << "cannot open data" << endl;
		return false;
	}
	string s;
	while (getline(infile, s)) {
		insert(root, s);
	}

	return true; // TODO
}







bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	if (search(root, word))
		return true;
	suggestions.clear();
	int count = 0;
	//string pre = "";
	//string check = "";
	for (int i = 0; i < word.size(); i++) { //by brute force, iterate through every letter in every position of word and search to see if its valid
		for (int j = 0; j < 28; j++) {
			char ch;
			if (j == 27) {
				ch = '\'';
			}
			else
				ch = 'a' + j;
			word[i] = ch;
			if (search(root, word)) {
				if (suggestions.size() == max_suggestions)
					return false;
				suggestions.push_back(word);
			}
		}
		//pre += word[i];
		if (suggestions.size() == max_suggestions)
			break;



	}
	return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	string s = "";
	vector<string> words;
	int start = 0;
	bool started = false;
	for (int i = 0; i < line.size(); i++) {


		if (line[i] != ' ' && isalpha(line[i])) { //if a alphabetical character is found
			if (!started) { //flag if its the start of a word
				start = i;
				started = true;
			}
			s += line[i];
			//continue;
		}
		if (!isalpha(line[i]) && s != "" || (isalpha(line[i]) && i ==line.size()-1)) { //if we are at the end of the line or a non alph character
			int end = i; //update end position
			
			started = false;
			if (isalpha(line[i])) //if we have character input, we're still on a word, flag back to true
				started = true;
			if (!search(root, s)) { //if word cant be found, append it to problems
				problems.push_back({ start,end });
				s = "";
			}
			if (!isalpha(line[i]) && s != "") //reset word if non alphabetic character was received 
				s = "";
			//continue;
		}

	}
}
