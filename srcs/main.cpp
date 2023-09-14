/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julmuntz <julmuntz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 18:09:16 by mbelrhaz          #+#    #+#             */
/*   Updated: 2023/09/14 14:45:12 by julmuntz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Exodus.hpp"
#include "RequestParser.hpp"

	// std::vector<std::string>	allow_methods;

// std::vector<std::string>	names;

// t1_location	loc_1 = {
// 	.uri = "/",
// 	.allow_methods = allow_methods,
// 	.root = "/html",
// 	.index = "index.html",
// 	.redir_link = "",
// 	.file_location = "/static",
// 	.cgi_pass = "",
// 	.directory_listing = true
// };

// t1_location	loc_2 = {
// 	.uri = "/color",
// 	.allow_methods = allow_methods,
// 	.root = "/color",
// 	.index = "coco.html",
// 	.redir_link = "https://google.com",
// 	.file_location = "/static",
// 	.cgi_pass = "",
// 	.directory_listing = false
// };

// std::vector<t1_location>	locations;

// t1_server	con_1 = {
// 		.listen = std::make_pair(8042, "localhost"),
// 		.server_name = names,
// 		.client_body_size = 10000,
// 		.error_page = std::make_pair(404, "/error/404.html"),
// 		.locations = locations
// }

// t1_server	con_2 = {
// 		.listen = std::make_pair(8043, "localhost"),
// 		.server_name = names,
// 		.client_body_size = 10000,
// 		.error_page = std::make_pair(404, "/error/404.html"),
// 		.locations = locations
// };

// std::vector<t1_server>	context_servers;

int main()
{
		std::string request;
		request = "\r\nTransfer-Encoding: chunked\r\n\r\n27\r\nVoici les données du premier morceau\r\n\r\n1C\r\net voici un second morceau\r\n\r\n20\r\net voici deux derniers morceaux \r\n12\r\nsans saut de ligne\r\n0\r\n\r\n";
	Server	webserver;
	int		ret;
	//Exodus pp("conf/default.conf");
	std::vector<t_server> context_servers;
	// names.push_back("ok");
	// names.push_back("lala");

	// allow_methods.push_back("GET");
	// allow_methods.push_back("DELETE");

	// locations.push_back(loc_1);
	// locations.push_back(loc_2)

	std::string tec;
	size_t pos = std::string::npos;
	pos = request.find("\r\nTransfer-Encoding: chunked\r\n");
	size_t posf = std::string::npos;
	posf = request.find("\r\n0\r\n\r\n");
	std::string l;
	if (pos != std::string::npos)
		if (posf != std::string::npos)
			l = request.substr(pos + 32, posf);
	std::cout << l ;
	while (true)
	{
		size_t p = std::string::npos;
		p = request.find("\r\n", 1);
		unsigned int i = 0;
		std::cout << p << std::endl;
		std::istringstream iss(l.substr(0, p));
		iss >> std::hex >> i;
		if (i == 0)
			break ;

		l.erase(0, p + 4);
		tec += l.substr(0, i);
		l.erase(0, i);
		if (!l[1] && !l[2] && !l[3] && l[4] &&
		l[1] == '\r' && l[2] == '\n' && l[3] == '\r' && l[4] == '\n')
		{
			tec += "\r\n";
			l.erase(0, 2);
		}
		l.erase(0, 2);

	}
	// while (!l.empty())
	// {
	// 	size_t p = std::string::npos;
	// 	p = request.find("\r\n");
	// 	unsigned int i = 0;
	// 	std::istringstream iss(l.substr(0, p));
	// 	iss >> std::hex >> i;
	// 	l.erase(0, p + 4);
	// 	p = request.find("\r\n");

	// 	std::cout << l.substr(0, ) ;
	// 	l = "";
	// }



	// requestParser rep(request);
	// std::cout << rep.get_methods() << " " << rep.get_uri() << " " << rep.get_version() << std::endl;
	// std::cout << rep.get_req_head().host << std::endl;
	// context_servers.push_back(con_2);
	try {
		Exodus pp("conf/conf1.conf");
		pp.setup();
		context_servers = pp.get_server();
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

	// for (long unsigned int i = 0; i < context_servers.size(); i++)
	// {
	// 	std::cout << "server {" << std::endl;
	// 	std::cout << "listen " << context_servers[i].listen.first << ":" << context_servers[i].listen.second << std::endl;
	// 	std::cout << "server_name";
	// 	for (int y = 0; y < 2; y++)
	// 		std::cout << " " << context_servers[i].server_name[y];
	// 	std::cout << std::endl;
	// 	std::cout << "client_body_size " << context_servers[i].client_body_size << std::endl;
	// 	std::cout << "error_page";
	// 	for (int y = 0; y < 1; y++)
	// 		std::cout << " " << context_servers[i].error_pages[y].first << " " << context_servers[i].error_pages[y].second;
	// 	std::cout << std::endl;

	// 	for(long unsigned int l = 0; l < context_servers[i].location.size(); l++)
	// 	{
	// 		std::cout << "	location " << context_servers[i].location[l].uri <<  "{" << std::endl;
	// 		std::cout << "		allow_methods";
	// 		for (int y = 0; y < 2; y++)
	// 			std::cout << " " << context_servers[i].location[l].allow_methods[y];
	// 		std::cout << std::endl;
	// 		std::cout << "		root " << context_servers[i].location[l].root << std::endl;
	// 		std::cout << "		index " << context_servers[i].location[l].index[0] << std::endl;
	// 		std::cout << "		redir_link " << context_servers[i].location[l].redir_link << std::endl;
	// 		std::cout << "		file_location " << context_servers[i].location[l].file_location << std::endl;
	// 		std::cout << "		cgi_pass " << context_servers[i].location[l].cgi_pass << std::endl;
	// 		if (context_servers[i].location[l].directory_listing)
	// 			std::cout << "		directory_listing true" << std::endl;
	// 		else
	// 			std::cout << "		directory_listing true" << std::endl;
	// 		std::cout << "	}" << std::endl;
	// 	}

	// 	std::cout << "}" << std::endl;
	// }
	webserver.set_con_servs(context_servers);
	ret = webserver.serve_do_your_stuff();
	return (ret);
}
