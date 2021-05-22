#pragma once

#include "utils.h"
#include "parse-args.h"
#include "file-utils.h"
#include "error.h"
#include "compatible-layer.h"
#include "sha256.h"
#include "config.h"

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <cmath>

using namespace std;

struct Cipher {
	struct CipherHeader {
		vector<uint8_t> keyHash, random, randomPerm;
		CipherHeader(const string& key) :
			keyHash{ hashingKey(key) },
			random{ getRandom() },
			randomPerm{ getRandom() }
		{
		}
		size_t size() {
			return keyHash.size() + random.size() + randomPerm.size();
		}

		vector<uint8_t> toBytes() {
			vector<uint8_t> res(size());
			auto curPos = res.begin();
			copy(keyHash.begin(), keyHash.end(), curPos);
			copy(random.begin(), random.end(), curPos += keyHash.size());
			copy(randomPerm.begin(), randomPerm.end(), curPos + random.size());
			return res;
		}

		void copyBytes(const vector<uint8_t>& bytes, size_t& curPos, vector<uint8_t>& dest) {
			auto end = curPos + dest.size();
			copy(bytes.begin() + curPos, bytes.begin() + end, dest.begin());
			curPos = end;
		}
		void fromBytes(const vector<uint8_t>& bytes, size_t start, size_t end) {
			size_t count = end - start;
			if (count != size())
				error("Bad header size");
			copyBytes(bytes, start, keyHash);
			copyBytes(bytes, start, random);
			copyBytes(bytes, start, randomPerm);
		}
		void cipherUncipherKeyHash() {
			xorByHash(keyHash, random);
		}
	private:
		vector<uint8_t> hashingKey(const string& key) {
			return sha256(key);
		}
	} header;

	string file;
	fstream finout;
	size_t len;
	size_t bufSize = BUFFER_SIZE;
	Cipher(const string& file, const string& key) : header{ key }, file{ file } {
		openFileForReadWrite(file, finout, len);
	}

	~Cipher()
	{
		finout.close();
	}

	//private:
	size_t getPosForHeader(size_t size) {
		auto hSize = header.size();
		auto factor = .5;
		if (hSize > size) return 0;
		auto half = (size - hSize) / 2;
		auto left = static_cast<size_t>(floor(half * factor));
		auto right = size - left;
		auto width = right - left - hSize;
		if (left == 0) return width / 2 + (width % 2);
		if (width < 2) return left;
		auto pred = size - 1;
		auto next = (pred * (pred + 1) / 2) * (size * (size + 1) / 2);
		auto pos = left + 1 + (next % width);
		return pos;
	}
private:
	void contentPerm(vector<uint8_t>& data, const vector<uint8_t>& perm) {
		size_t byBits = perm.size() * 8;
		size_t halfLen = data.size() / 2;
		for (size_t i = 0; i < halfLen; ++i) {
			if (getBit(i % byBits, perm)) {
				swap(data[i], data[i + halfLen]);
			}
		}
	}

	void cipherByHash(vector<uint8_t>& data)
	{
		xorByHash(data, header.keyHash);
	/*	for (size_t i = 0; i < data.size(); ++i)
		{
			data[i] ^= header.keyHash[i % header.keyHash.size()];
		}*/
	}

public:
	void cipherData(vector<uint8_t>& data) {
		cipherByHash(data);
		contentPerm(data, header.randomPerm);
	}

	void uncipherData(vector<uint8_t>& data) {
		contentPerm(data, header.randomPerm);
		cipherByHash(data);
	}

	void cipherFile() {
		if (len > 0) {
			vector<uint8_t> data(bufSize);
			for(size_t i = 0; i < len - bufSize; i += bufSize) {
				readData(finout, data, 0, bufSize, i, bufSize);
				cipherData(data);
				writeData(data, finout, 0, bufSize, i);
			}
			auto wholeParts = bufSize * (len / bufSize);
			auto restSize = len - wholeParts;
			if (restSize != 0) {
				vector<uint8_t> data(restSize);
				readData(finout, data, 0, restSize, wholeParts, restSize);
				cipherData(data);
				writeData(data, finout, 0, data.size(), wholeParts);
			}

			
		}

		header.cipherUncipherKeyHash();
		auto headerBytes = header.toBytes();
		writeData(headerBytes, finout, 0, headerBytes.size(), len);

		if (len > 0)
			moveHeader(true, len + headerBytes.size());
	}

	void uncipherFile() {
		if (len > 3)
			moveHeader(false, len);

		// Load header
		auto headerSz = header.size();
		vector<uint8_t> headerBytes(headerSz);

		readData(finout, headerBytes, 0, headerSz, len - headerSz, headerSz);
		header.fromBytes(headerBytes, 0, headerSz);
		header.cipherUncipherKeyHash();

		// Load data
		auto dataSz = len - headerSz;
		vector<uint8_t> dataBytes(bufSize);
		for (size_t i = 0; i < dataSz - bufSize; i += bufSize) {
			readData(finout, dataBytes, 0, bufSize, i, bufSize);
			uncipherData(dataBytes);
			// Write
			//if (!dataBytes.empty())
			writeData(dataBytes, finout, 0, bufSize, i);
		}
		auto wholeParts = bufSize * (dataSz / bufSize);
		auto restSize = dataSz - wholeParts;
		if (restSize != 0) {
			vector<uint8_t> dataBytes(restSize);
			readData(finout, dataBytes, 0, restSize, wholeParts, restSize);
			uncipherData(dataBytes);
			// Write
			//if (!dataBytes.empty())
			writeData(dataBytes, finout, 0, restSize, wholeParts);
		}
		auto newSz = len - header.size();
		truncate(file.c_str(), newSz);
	}

	void moveHeader(bool bFromEnd, size_t sz)
	{// Refactor it
		auto hSize = header.size();
		auto dSize = sz - hSize;
		auto intoPos = getPosForHeader(sz);
		auto dRightSize = dSize - intoPos;
		vector<uint8_t> headerBytes(hSize);

		if (bFromEnd) {
			auto hStart = sz - hSize;
			readData(finout, headerBytes, hStart);
			if (intoPos > dSize)
				error("Failed positioning");
			if (intoPos == dSize)
				return;
			
			vector<uint8_t> rightData(dRightSize);
			readData(finout, rightData, intoPos);
			writeData(headerBytes, finout, intoPos);
			writeData(rightData, finout, intoPos + hSize);
		}
		else {
			readData(finout, headerBytes, intoPos);
			auto dRightStart = intoPos + hSize;
			vector<uint8_t> rightData(dRightSize);
			readData(finout, rightData, dRightStart);
			writeData(headerBytes, finout, dSize);
			writeData(rightData, finout, intoPos);
		}
	}
};