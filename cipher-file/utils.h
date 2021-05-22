#pragma once

#include "config.h"

#include <ctime>
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <cstring>
#include <ctime>

using namespace std;

char* asOldArray(const vector<uint8_t>& ar);
bool oldInitRand();
mt19937& initRand();
vector<uint8_t> getRandom();
vector<uint8_t> toBytes(const string& data);
string fromBytes(const vector<uint8_t>& data);
bool getBit(size_t bitIdx, const vector<uint8_t>& data);
void xorByHash(vector<uint8_t>& data, const vector<uint8_t>& hash);