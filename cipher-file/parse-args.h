#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

enum class ECipher {
	none, cipher, uncipher
};

struct ParsedArgs {
	// Cipher/Undo_cipher
	ECipher cipher = ECipher::none;
	string output;
	string key;
};

bool isKeyParam(const string& param);
void getHelp();
void parseParams(ParsedArgs& args, const string& predParam, const string& param);
ParsedArgs parseArgs(int argc, char** argv);