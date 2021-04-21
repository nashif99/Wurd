#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <stack>
using namespace std;
class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct Change { //create a struct containing all the info we need to record an undo submission
		Action act;
		int nrow;
		int ncol;
		string c;
	};
	stack<Change*> u_stack; //use a stack of Change* to log each submit
};

#endif // STUDENTUNDO_H_
