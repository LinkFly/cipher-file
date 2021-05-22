#pragma once

#include "error.h"

#if defined(_WIN32) || defined(_WIN64)

#include <io.h>
#include <fcntl.h>
#else
#ifdef POSIX
#include <unistd.h>
#else
// code for other OSes
#endif
#endif

using namespace std;

void truncate(const char* file, long size)
{
#if defined(_WIN32) || defined(_WIN64)
	int fd;
	auto res = _sopen_s(&fd, file, _O_WRONLY, _SH_DENYNO, _S_IWRITE);
#ifdef POSIX
	auto fd = _open(file, O_WRONLY);
#else
// code for other OSes
#endif
#endif
	
	if (fd <= 0)
		error("Failed open file");
	
#if defined(_WIN32) || defined(_WIN64)
	res = _chsize(fd, size);
#else
#ifdef POSIX
	res = ftruncate(fd, size);
#else
	// code for other OSes
#endif
#endif
	if (res != 0)
		error("Failed truncate file");
}