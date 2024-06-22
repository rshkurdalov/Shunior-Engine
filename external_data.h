#pragma once
#include "string.h"

string &operator<<(string &target, const char8 *source);
string &operator<<=(string &target, const char8 *source);
string &operator<<(string &target, const char16 *source);
string &operator<<=(string &target, const char16 *source);
char8 *create_u8sz(string &str);
char16 *create_u16sz(string &str);
uint64 u8sz_length(const char8 *str);
uint64 u16sz_length(const char16 *str);
