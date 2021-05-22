#pragma once

//#include "utils.h"
//#include "parse-args.h"
//#include "file-utils.h"
//#include "error.h"
//#include "compatible-layer.h"
//#include "sha256.h"
#include "config.h"
#include "CipherHeader.h"

//#include <iostream>
#include <vector>
#include <cstdint>
//#include <cstdlib>
#include <fstream>
//#include <algorithm>
//#include <cmath>

using namespace std;

struct Cipher {
	CipherHeader header;
	string file;
	fstream finout;
	size_t len;
	size_t bufSize = BUFFER_SIZE;
	Cipher(const string& file, const string& key);
	~Cipher();
	size_t getPosForHeader(size_t size);
private:
	void contentPerm(vector<uint8_t>& data, const vector<uint8_t>& perm);
	void cipherByHash(vector<uint8_t>& data);

public:
	void cipherData(vector<uint8_t>& data);
	void uncipherData(vector<uint8_t>& data);
	void cipherFile();
	void uncipherFile();
	void moveHeader(bool bFromEnd, size_t sz);
};