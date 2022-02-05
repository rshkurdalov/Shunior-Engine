#include "web_server.h"
#include "os_api.h"

web_server::web_server()
{
	state = web_server_state::closed;
}

web_server::~web_server()
{

}

void web_server::run(string &uri_value)
{
	uri << uri_value;
	os_regiser_web_server(this);
	state = web_server_state::running;
}

void web_server::close()
{
	os_unregister_web_server(this);
	state = web_server_state::closed;
}
