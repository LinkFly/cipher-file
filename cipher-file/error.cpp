#include "error.h"

void error(const string& msg)
{
	cout << msg << endl;
	exit(-1);
}
