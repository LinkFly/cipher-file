#pragma once

#include "utils.h"
#include "sha256.h"

#include <cstdint>
#include <vector>

using namespace std;

struct CipherHeader {
	vector<uint8_t> keyHash, random, randomPerm;
	CipherHeader(const string& key);
	size_t size();
	vector<uint8_t> toBytes();
	void copyBytes(const vector<uint8_t>& bytes, size_t& curPos, vector<uint8_t>& dest);
	void fromBytes(const vector<uint8_t>& bytes, size_t start, size_t end);
	void cipherUncipherKeyHash();
private:
	vector<uint8_t> hashingKey(const string& key);
};