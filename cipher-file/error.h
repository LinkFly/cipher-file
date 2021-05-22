#pragma once

#include <string>
#include <iostream>

using namespace std;

void error(const string& msg) {
	cout << msg << endl;
	exit(-1);
}