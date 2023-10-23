#include "Server.hpp"
#include "Exodus.hpp"

int main(int argc, char ** argv)
{
	if (argc < 2 || argc > 2)
		return (cerr << "./webserv [configuration file]" << endl, 0);

	Server				webserver;
	int					ret;
	vector<t_server>	context_servers;

	try {
		Exodus pp(argv[1]);
		pp.setup();
		context_servers = pp.get_server();
		webserver.set_con_servs(context_servers);
		ret = webserver.serve_do_your_stuff();
	}
	catch(const exception &e)
	{
		cerr << e.what() << endl;
	}
	return (ret);
}
