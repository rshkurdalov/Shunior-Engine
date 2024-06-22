#pragma once
#include "array.h"
#include "string.h"
#include "file.h"

enum struct snode_type
{
	string,
	array
};

struct snode
{
	string name;
	snode_type type;
	string value;
	array<snode> elements;

	snode();
};

template<> struct utility<snode>
{
	void copy(const snode &source, snode *target);
};

struct structured_file
{
	snode root;

	void load(string &filename);
	void clear();
	void save(string &filename);
};
