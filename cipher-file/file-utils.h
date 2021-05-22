#pragma once

#include "utils.h"
#include "error.h"

#include <fstream>
#include <memory>
#include <cstdint>
#include <vector>

using namespace std;

void openFileForReadWrite(const string& file, fstream& finout, size_t& len) {
	finout.open(file, ios::ate | ios::in | ios::out | ios::binary);
	if (!finout) {
		error("Failed open file for write: " + file);
	}
	//finout.setstate(ios::exceptions);
	len = static_cast<size_t>(finout.tellg());
	finout.seekg(0, ios::beg);
	//finout.rdbuf()->fd
}

void openFileForRead(const string& file, ifstream& fin, size_t& len) {
	fin.open(file, ios::ate);
	if (!fin) {
		error("Failed open file for write: " + file);
	}
	len = static_cast<size_t>(fin.tellg());
	fin.seekg(0, ios::beg);
}

shared_ptr<ifstream> openFileForRead(const string& file, size_t& len) {
	// Get ifstream
	auto deleter = [](ifstream* pf) {
		pf->close();
		delete pf;
	};
	shared_ptr<ifstream> pfin(new ifstream(), deleter);
	auto& fin = *pfin;
	openFileForRead(file, fin, len);
	// end Get ifstream

	len = static_cast<size_t>(fin.tellg());
	fin.seekg(0, ios::beg);
	return pfin;
}

size_t fileSize(istream& in) {
	auto curPos = in.tellg();
	in.seekg(0, ios::end);
	auto res = in.tellg();
	in.seekg(curPos, ios::beg);
	return static_cast<size_t>(res);
}

inline void checkPartSizes(
				istream& in, const vector<uint8_t>& data, 
				size_t toStart, size_t toEnd,
				size_t start, size_t count)
{
	auto len = fileSize(in);
	if (toEnd - toStart != count)
		error("Bad part of data (count not equal part size)");
	if (toStart + count > data.size())
		error("Failed read data - array is smaller size");
	if (start + count > len)
		error("Failed read data - file is smaller size");
}

void readData(istream& in, vector<uint8_t>& data,
				size_t toStart, size_t toEnd, 
				size_t start, size_t count)
{
	checkPartSizes(in, data, toStart, toEnd, start, count);
	auto pData = reinterpret_cast<char*>(data.data());
	pData += toStart;
	in.seekg(start, ios::beg);
	in.read(pData, count);
	if (in.fail() || in.bad()) {
		error("failed read data");
	}
}

void readData(istream& in, vector<uint8_t>& data, size_t start) {
	readData(in, data, 0, data.size(), start, data.size());
}

vector<uint8_t> readFile(const string& file) {
	size_t len;
	auto pfin = openFileForRead(file, len);
	auto& fin = *pfin;
	vector<uint8_t> res(len);
	readData(fin, res, 0, len, 0, len);
	return res;
}

void openFileForWrite(const string& file, fstream& fout) {
	fout.open(file);
	if (!fout) {
		error("Failed open file for write: " + file);
	}
}

inline void checkWriteSizes(const vector<uint8_t>& data,
							size_t fromStart,
							size_t fromEnd,
							size_t start,
							size_t curLen)
{
	if ((fromEnd - fromStart <= 0) || fromEnd > data.size())
		error("Bad size of part of data");
	if (start > curLen) {
		error("Bad start position (greater than curLen)");
	}
}

void writeData(const vector<uint8_t>& data, fstream& out,
				size_t fromStart, size_t fromEnd,
				size_t start)
{
	auto curLen = fileSize(out);
	checkWriteSizes(data, fromStart, fromEnd, start, curLen);
	auto count = fromEnd - fromStart;
	auto pData = asOldArray(data) + fromStart;
	//out.seekg(0, ios::beg);
	out.seekp(start, ios::beg);
	out.write(pData, count);
	if (out.fail() || out.bad()) {
		
		error("failed wrote data");
	}
}

void writeData(const vector<uint8_t>& data, fstream& fout, size_t start) {
	writeData(data, fout, 0, data.size(), start);
}

void writeFile(const vector<uint8_t>& data, const string& file,
	size_t start = 0, size_t count = 0)
{
	if (count == 0)
		count = data.size();
	// Get ofstream
	auto deleter = [](fstream* pf) {
		pf->close();
		delete pf;
	};
	unique_ptr<fstream, decltype(deleter)> pfout(new fstream{}, deleter);
	// end Get ofstream

	openFileForWrite(file, *pfout);
	writeData(data, *pfout, 0, count, start);
}

//void checkParts(size_t start1, size_t end1, size_t start2, size_t end2) {
//
//}
//void moveBytes(fstream& inout, size_t start1, size_t end1, size_t start2, size_t end2) {
//	if (end1 < start1 || end2 < start2)
//		error("Failed: end less start");
//	if (end1 - start1 != end2 - start2) 
//		error("Failed: different part size");
//	auto len = end1 - start1;
//	vector<uint8_t> buf1(len), buf2(len);
//
//
//	readData(inout, buf1, 0, len, start1, len);
//	readData(inout, buf2, 0, len, start2, len);
//	writeData(buf1, inout, 0, len, start2);
//	writeData(buf2, inout, 0, len, start1);
//}