#pragma once
#include "array.h"

typedef array<char32> string;

bool operator<(const string &value1, const string &value2);
bool operator==(const string &value1, const string &value2);
bool operator!=(const string &value1, const string &value2);
bool operator>(const string &value1, const string &value2);
void operator<<(string &target, const char8 *source);
void operator<<(string &target, const char16 *source);
void operator<<(string &target, const char32 *source);
