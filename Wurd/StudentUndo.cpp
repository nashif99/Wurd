#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	if (!u_stack.empty()) {
		Change* temp = u_stack.top();
		if (temp->act == Action::DELETE && action == Action::DELETE && temp->ncol == col && temp->nrow == row) { //if delete was pressed at the same position multiple times
			u_stack.pop(); //pop the top of the stack, edit it, and put it back
			temp->c += ch;
			u_stack.push(temp);
			return;
 		}
		else if (temp->act == Action::DELETE && action == Action::DELETE && temp->ncol == col + 1 && temp->nrow == row) { //if backspace pressed consecutively 
			u_stack.pop(); //pop the top of the stack, edit it, and put it back
			string first = (ch+ temp->c);
			temp->c = first;
			temp->ncol = col;
			u_stack.push(temp);
			return;
		}
		else if (temp->act == Action::INSERT && action == Action::INSERT && temp->ncol == col - 1 && temp->nrow == row) {
			u_stack.pop(); //pop the top of the stack, edit it, and put it back
			string first = (ch + temp->c);
			temp->c = first;
			temp->ncol = col;
			u_stack.push(temp);
			return;
		}
	}
	Change*s = new Change; //create a new Change object, assign parameters passed in, and add to stack
	s->act = action;
	s->nrow = row;
	s->ncol = col;
	s->c = ch;
	u_stack.push(s);
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
	if (u_stack.empty())
		return Action::ERROR;
	Change* t = u_stack.top(); //get the Change object from the top of the stack and
	u_stack.pop();
	row = t->nrow;
	col = t->ncol;
	text = t->c;
	count = 1;
	if (t->act == Action::INSERT) //return opposite action than received 
		return Action::DELETE;
	if (t->act == Action::DELETE)
		return Action::INSERT;
	if (t->act == Action::SPLIT)
		return Action::JOIN;
	if (t->act == Action::JOIN)
		return Action::SPLIT;

    return Action::ERROR;  // TODO
}

void StudentUndo::clear() {
	while (!u_stack.empty()) {
		delete u_stack.top();
		u_stack.pop();
	}
}
