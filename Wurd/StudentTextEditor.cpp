#include "StudentTextEditor.h"
#include "Undo.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo),m_row(0),m_col(0) {
	//words.push_back("");
	glob_it = words.begin();

}

StudentTextEditor::~StudentTextEditor()
{

	// TODO
}

bool StudentTextEditor::load(std::string file) {
	reset();
	ifstream infile(file);
	if (!infile) {
		cerr << "cannot open data" << endl;
		return false;
	}
	string s;
	while (getline(infile, s)) {
		if (s.size() > 0) {
			if (s[s.size() - 1] == '\n') { //check if there is a newline character at the end
				if (s.size() >= 2) //if there is a new line character and there are 2 or more characters
					if (s[s.size() - 2] == '\r') //check if the \r character before newline
						s.erase(s.size() - 2);//strip the \r
			}
			
		}
		words.push_back(s);
	}
	glob_it = words.begin(); //reset the global iterator

	return true;  // TODO
}

bool StudentTextEditor::save(std::string file) { //not sure if this works yet
	ofstream outfile(file);
	if (!outfile) {
		cerr << "Error:cannot create file" << endl;
		return false;
	}
	list<string>::iterator it = words.begin();
	while (it != words.end()) {	//iterate through the lines and add them to the outfile
		outfile << *it << endl;
		it++;
	}
	return true;
}

void StudentTextEditor::reset() {
	std::list<std::string>::iterator it = words.begin();
	while (it != words.end()) {
		it = words.erase(it);
	}
	glob_it = words.begin();
	m_col = 0;
	m_row = 0;
	getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
	if (dir == Dir::UP) {
		if (m_row == 0)
			return;
		m_row--; //go up
		/*if (m_row == 0) {
			glob_it = words.begin(); //back at beginning of file
			return;
		}*/
		glob_it--; //decrement iterator to go back to previous line  
		if (*glob_it == "" || m_col > glob_it->size()) //if the current line is longer than previous line, go to last position in previous line
			m_col = glob_it->size();
	}
	if (dir == Dir::DOWN) {
		if (m_row >= words.size()-1|| words.size()==0) //do nothing if we are at end of file
			return;
		m_row++; //go down
		glob_it++; //inc iterator to next line 
		if (glob_it->size() == 0)
			return;
		if (*glob_it == "" || m_col>glob_it->size()) //if current line is longer than next line,
			m_col = glob_it->size();//set the col position to end of next line
	}
	if (dir == Dir::LEFT) {
		if (m_col == 0) { //if we are at the beginning of a line
			if (m_row == 0) // do nothing at the beginning of a file
				return;
			glob_it--; //go to previous line and set the col position to end of previous line
			m_row--;
			m_col = glob_it->size();
			return;
		}
		else		
			m_col--;
	}
	if (dir == Dir::RIGHT) {
		if (m_col == glob_it->size()) {
			if (m_row == words.size() - 1)
				return;
			m_row++;
			m_col = 0;
			glob_it++;
		}
		else
			m_col++;
	}
}

void StudentTextEditor::del() {
	if (words.empty())
		return;
	if (m_col != glob_it->size()) {
		getUndo()->submit(Undo::Action::DELETE, m_row, m_col, (*glob_it)[m_col]);
		glob_it->erase(m_col,1); //erase one element, O(new string length) 

	}
	else if(m_row!=words.size()-1) { //if the cursor is at the end of a line, store the next line, append to current string, erase the appended line
		glob_it++;
		string temp = *glob_it; 
		glob_it = words.erase(glob_it);
		glob_it--;
		getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
		(*glob_it) += temp;
	}
}

void StudentTextEditor::backspace() {
	if (words.empty())
		return;
	if (m_col > 0) {
		getUndo()->submit(Undo::Action::DELETE, m_row, m_col - 1, (*glob_it)[m_col - 1]);
		glob_it->erase(m_col - 1, 1);//erase the one charcter before the current position
		m_col--;
	}
	else if ((m_col == 0 && m_row!=0) || (glob_it->size()==0 &&m_row!=0)) { // if it is not the first line and backspace is called at beginning of line
		string temp = *glob_it;
		glob_it = words.erase(glob_it); //temporarily store the line and then delete it
		glob_it--;
		m_col = glob_it->size();
		m_row--;
		getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
		(*glob_it) += temp; //append the line before with the deleted line
	}


}

void StudentTextEditor::insert(char ch) {

	string s = ""; //create an empty string, convert the character to a string
	if (ch == '\t') {
		s = "    ";
		if (words.empty()) { //handles inserting a tab of empty text editor
			words.push_back(s);
			glob_it = words.begin();
			m_col += 4;
			//getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
			//return;
		}
		else {
			glob_it->basic_string::insert(m_col, s);
			m_col += 4;
		}
		for (int i = 0; i < 4; i++) {
			getUndo()->submit(Undo::Action::INSERT, m_row, m_col-i, ch);
		}
		return;

	}
	s += ch;
	if (words.empty()) { //case of inserting character to new text editor
		words.push_back(s);
		glob_it = words.begin();
		m_col++;
		getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
		return;
	}
	glob_it->string::insert(m_col, s); //insert the character at the current position
	m_col++;
	getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
	//std::string::insert runs at linear time 
}

void StudentTextEditor::enter() {

	if (words.size() == 0) { // if the file is empty and we want to enter
		glob_it = words.insert(glob_it, "\n");
		m_row++;
		glob_it++;
		glob_it = words.insert(glob_it, "");
		return;
	}
	getUndo()->submit(Undo::Action::SPLIT, m_row, m_col);
	string temp1 = (*glob_it).substr(m_col);//split string from col pos, O(length of substr)
	*glob_it= (*glob_it).substr(0, m_col); //get first part of string, O(total string length - temp1)
	// total should be O(length of line)
	glob_it++;
	glob_it=words.insert(glob_it, temp1); //O(1) since we kept track of position
	m_row++;
	m_col = 0;
	// TODO
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = m_row; col = m_col; // TODO
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	if (startRow < 0 || numRows <0 || startRow > words.size())
		return -1;
	list<string>::const_iterator it = words.begin();
	int count = 0;
	lines.clear();
	for (int i = 0; i < words.size(); i++) { //iterate through the words, start counting at the startRow and count for numRow iterations
		if (it != words.end()) {
			if (count == numRows) {
				break;
			}
			if (i >= startRow) {
				lines.push_back(*it);
				count++;
			}
			it++;
		}
	}
	return lines.size(); // TODO
}

void StudentTextEditor::undo() {
	if (words.empty())
		return;
	string to_undo; //get the position and string from get
	int u_row;
	int u_col;
	int u_count;
	Undo::Action undo = getUndo()->get(u_row, u_col, u_count, to_undo);
	if (undo == Undo::Action::ERROR)
		return;
	int rowdiff = m_row - u_row;
	if (rowdiff > 0) {
		for (int i = 0; i < rowdiff; i++) {
			glob_it--;
		}
	}
	if (rowdiff < 0) {
		for (int i = 0; i < abs(rowdiff); i++) {
			glob_it++;
		}
	}
	m_row = u_row;
	m_col = u_col;
	if (undo == Undo::Action::INSERT) {
		for (int i = 0; i < u_count; i++) { //u_count is 1 anyway, loop is unnecessary run time still O(1). but leaving it for now
			glob_it->string::insert(m_col, to_undo); //insert the character at the current position
			//m_col++;
		}
	}
	//m_col = u_col;
	if (undo == Undo::Action::DELETE) {
		for (int i = 0; i < to_undo.size(); i++) { //delete string
			glob_it->erase(m_col-1, 1);
			m_col--;
		}

	}
	if (undo == Undo::Action::SPLIT) {
		string temp1 = (*glob_it).substr(m_col);//split string from col pos, O(length of substr)
		*glob_it = (*glob_it).substr(0, m_col); //get first part of string, O(total string length - temp1)
		// total should be O(length of line)
		glob_it++;
		glob_it = words.insert(glob_it, temp1); //O(1) since we kept track of position
		m_row++;
		m_col = 0;
	}
	if (undo == Undo::Action::JOIN) {
		string temp = *glob_it;
		glob_it = words.erase(glob_it); //temporarily store the line and then delete it
		glob_it--;
		m_col = glob_it->size();
		m_row--;
		//getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
		(*glob_it) += temp; //append the line before with the deleted line
	}
}
