#include "CipherHeader.h"

#include "utils.h"
#include "sha256.h"
#include "error.h"

#include <cstdint>
#include <vector>

using namespace std;

CipherHeader::CipherHeader(const string& key) :
	keyHash{ hashingKey(key) },
	random{ getRandom() },
	randomPerm{ getRandom() }
{

}

size_t CipherHeader::size()
{
	return keyHash.size() + random.size() + randomPerm.size();
}

std::vector<uint8_t> CipherHeader::toBytes()
{
	vector<uint8_t> res(size());
	auto curPos = res.begin();
	copy(keyHash.begin(), keyHash.end(), curPos);
	copy(random.begin(), random.end(), curPos += keyHash.size());
	copy(randomPerm.begin(), randomPerm.end(), curPos + random.size());
	return res;
}

void CipherHeader::copyBytes(const vector<uint8_t>& bytes, size_t& curPos, vector<uint8_t>& dest)
{
	auto end = curPos + dest.size();
	copy(bytes.begin() + curPos, bytes.begin() + end, dest.begin());
	curPos = end;
}

void CipherHeader::fromBytes(const vector<uint8_t>& bytes, size_t start, size_t end)
{
	size_t count = end - start;
	if (count != size())
		error("Bad header size");
	copyBytes(bytes, start, keyHash);
	copyBytes(bytes, start, random);
	copyBytes(bytes, start, randomPerm);
}

void CipherHeader::cipherUncipherKeyHash()
{
	xorByHash(keyHash, random);
}

std::vector<uint8_t> CipherHeader::hashingKey(const string& key)
{
	return sha256(key);
}
