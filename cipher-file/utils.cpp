#include "utils.h"

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

char* asOldArray(const vector<uint8_t>& ar)
{
	return const_cast<char*>(reinterpret_cast<const char*>(ar.data()));
}

bool oldInitRand()
{
	srand(static_cast<unsigned>(time(nullptr)));
	return true;
}

std::mt19937& initRand()
{
	static std::random_device rd;  //Will be used to obtain a seed for the random number engine
	static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	return gen;
}

std::vector<uint8_t> getRandom()
{
	constexpr auto RND_BYTES = 8;
	vector<uint8_t> res(RND_BYTES);
	if (USE_NEW_RANDOM) {
		static auto& gen = initRand();
		static std::uniform_int_distribution<uint64_t> distrib(0);
		uint64_t randomNum = distrib(gen);
		memcpy(res.data(), &randomNum, RND_BYTES);
	}
	else {
		static auto dontuse = oldInitRand();
		// Because we known that pData point to 8 byte
		uint64_t& data = *reinterpret_cast<uint64_t*>(res.data());
		data |= rand();
		for (int i = 0; i < 3; ++i) {
			data <<= 15;
			data |= rand();
		}
		data <<= 4;
		data |= rand() & 0b1111;
	}
	return res;
}

std::vector<uint8_t> toBytes(const string& data)
{
	return vector<uint8_t>(data.begin(), data.end());
}

std::string fromBytes(const vector<uint8_t>& data)
{
	return string(data.begin(), data.end());
}

bool getBit(size_t bitIdx, const vector<uint8_t>& data)
{
	auto shift = 1 << bitIdx % 8;
	return data[bitIdx / 8] & shift;
}

void xorByHash(vector<uint8_t>& data, const vector<uint8_t>& hash)
{
	for (size_t i = 0; i < data.size(); ++i)
	{
		data[i] ^= hash[i % hash.size()];
	}
}
