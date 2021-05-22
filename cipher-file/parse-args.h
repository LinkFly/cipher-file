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
	// Cipher/Uncipher
	ECipher cipher = ECipher::none;
	string output;
	string key;
};

bool isKeyParam(const string& param) {
	return param[0] == '-';
}

void getHelp() {
	string msg = "Using: <program> [-h] -c|-u -k <key> -o <file>\n"
" -h - Output this message\n"
" -c - Cipher file? (if it exist don't use -u)\n"
" -u - Undo cipher file? (if it exist don't use -c)\n"
" -k <key> - Key for cipher or undo cipher file (password)\n"
" -o <file> - File that required ciphering/undo ciphering\n"
;
	cout << msg << endl;
	exit(0);
}

void parseParams(ParsedArgs& args, const string& predParam, const string& param) {
	static const vector<string> allowKeys = { "-h", "-c", "-u", "-k", "-o" };
	if ((predParam == "-o" || predParam == "-k") && isKeyParam(predParam) && isKeyParam(param))
		getHelp();
	if (isKeyParam(predParam))
	{
		if (find(allowKeys.begin(), allowKeys.end(), predParam) == allowKeys.end())
			getHelp();
	}
	if ("-h" == param)
		getHelp();
	if ("-c" == param)
	{
		if (args.cipher == ECipher::uncipher)
			getHelp();
		args.cipher = ECipher::cipher;
		return;
	}
	if ("-u" == param)
	{
		if (args.cipher == ECipher::uncipher)
			getHelp();
		args.cipher = ECipher::uncipher;
		return;
	}
	if ("-k" == predParam)
	{
		args.key = param;
		return;
	}
	if ("-o" == predParam)
	{
		args.output = param;
		return;
	}
}

ParsedArgs parseArgs(int argc, char** argv) {
	if (!(argc > 1)) getHelp();
	ParsedArgs res;
	string predParam;
	for (int i = 1; i < argc; ++i) {
		string curParam = argv[i];
		parseParams(res, predParam, argv[i]);
		predParam = curParam;
	}
	if (res.cipher == ECipher::none || res.key.empty() || res.output.empty())
		getHelp();
	return res;
}