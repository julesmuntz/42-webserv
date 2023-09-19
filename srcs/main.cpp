#include "Server.hpp"
#include "Exodus.hpp"

// vector<t_server> serv;
// RequestParser rep("eee");

// t_server	choice_server()
// {
// 	t_server tmp;

// 	for (vector<t_server>::iterator it = serv.begin(); it != serv.end(); it++)
// 	{
// 		if (it->listen.first == rep.get_req_head().hosts.second && it->listen.second == rep.get_req_head().hosts.first)
// 			tmp = *it;
// 	}
// 	return (tmp);
// }

// size_t pos = std::string::npos;
	// pos = request.find("\r\n\r\n");
	// size_t posf = std::string::npos;
	// posf = request.find("\r\n0\r\n\r\n");
	// std::string body;
	// if (pos != std::string::npos)
	// 	if (posf != std::string::npos)
	// 		body = request.substr(pos + 4, posf - (pos + 4));
	// // std::cout << body;
	// pos = 0;
	// std::string unchunked_body = "";
	// std::string hexa_str;
	// size_t 		hexa;
	// std::string data_str;

	// while (true)
	// {
	// 	size_t end_pos = body.find("\r\n");
	// 	if (end_pos != std::string::npos)
	// 	{
	// 		hexa_str = body.substr(0, end_pos);
	// 		body.erase(0, end_pos + 2);
	// 		std::istringstream iss(hexa_str);
	// 		iss >> std::hex >> hexa;
	// 		data_str = body.substr(0, hexa);
	// 		unchunked_body += data_str;
	// 		if (hexa + 2 != std::string::npos)
	// 			body.erase(0, hexa + 2);
	// 	}
	// 	else
	// 		break;
	// }
	// std::cout << "NEW BODY\n" << unchunked_body << std::endl;

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
	}
	catch(const exception &e)
	{
		cerr << e.what() << endl;
	}
	webserver.set_con_servs(context_servers);
	ret = webserver.serve_do_your_stuff();
	return (ret);
}