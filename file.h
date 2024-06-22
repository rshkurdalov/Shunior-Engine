#pragma once
#include "string.h"

enum struct filestatus
{
	opened,
	closed
};

struct file
{
	string filename;
	bool read_access;
	bool write_access;
	filestatus status;
	uint64 position;
	uint64 size;
	void *handler;

	file();
	bool exists();
	void open();
	void close();
	void resize(uint64 file_size);
	uint64 read(uint64 bytes_size, void *addr);
	uint64 write(void *addr, uint64 bytes_size);
	bool remove();
};
