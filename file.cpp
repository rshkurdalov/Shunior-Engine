#include "file.h"
#include "os_api.h"

file::file()
{
	status = filestatus::closed;
}

bool file::exists()
{
	return os_filename_exists(filename);
}

void file::open()
{
	os_open_file(this);
}

void file::close()
{
	os_close_file(this);
}

void file::resize(uint64 file_size)
{
	os_resize_file(file_size, this);
}

uint64 file::read(uint64 bytes_size, void *addr)
{
	return os_read_file(this, bytes_size, addr);
}

uint64 file::write(void *addr, uint64 bytes_size)
{
	return os_write_file(this, addr, bytes_size);
}

bool file::remove()
{
	return os_delete_file(filename);
}
