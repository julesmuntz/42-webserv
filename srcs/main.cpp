/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbelrhaz <mbelrhaz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 18:09:16 by mbelrhaz          #+#    #+#             */
/*   Updated: 2023/08/20 18:10:46 by mbelrhaz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
// #include "Exodus.hpp"

std::vector<std::string>	allow_methods;

std::vector<std::string>	names;

t1_location	loc_1 = {
	.uri = "/",
	.allow_methods = allow_methods,
	.root = "/html",
	.index = "index.html",
	.redir_link = "",
	.file_location = "/static",
	.cgi_pass = "",
	.directory_listing = true
};

t1_location	loc_2 = {
	.uri = "/color",
	.allow_methods = allow_methods,
	.root = "/color",
	.index = "coco.html",
	.redir_link = "https://google.com",
	.file_location = "/static",
	.cgi_pass = "",
	.directory_listing = false
};

std::vector<t1_location>	locations;

t1_server	con_1 = {
		.listen = std::make_pair(8042, "localhost"),
		.server_name = names,
		.client_body_size = 10000,
		.error_page = std::make_pair(404, "/error/404.html"),
		.locations = locations
};

t1_server	con_2 = {
		.listen = std::make_pair(8043, "localhost"),
		.server_name = names,
		.client_body_size = 10000,
		.error_page = std::make_pair(404, "/error/404.html"),
		.locations = locations
};

std::vector<t1_server>	context_servers;

int main()
{
	Server	webserver;
	int		ret;
	//Exodus pp("conf/default.conf");

	names.push_back("ok");
	names.push_back("lala");

	allow_methods.push_back("GET");
	allow_methods.push_back("DELETE");

	locations.push_back(loc_1);
	locations.push_back(loc_2);

	context_servers.push_back(con_1);
	context_servers.push_back(con_2);

	// try {
	// 	pp.get_Exodus();
	// }
	// catch(const std::exception &e)
	// {
	// 	std::cerr << e.what() << std::endl;
	// }
	// std::cout << "Port is: " << pp.get_parss()[0].listen << std::endl;
	// std::cout << "Server names are: " << pp.get_parss()[0].server_name << std::endl;
	webserver.set_con_servs(context_servers);
	ret = webserver.serve_do_your_stuff();
	return (ret);
}
