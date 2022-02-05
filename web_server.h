#pragma once
#include "string.h"

enum struct web_server_state
{
	closed,
	running
};

struct web_server
{
	web_server_state state;
	string uri;
	void *handler;

	web_server();
	~web_server();
	void run(string &uri_value);
	void close();
};
