/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eflaquet <eflaquet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/20 18:09:16 by mbelrhaz          #+#    #+#             */
/*   Updated: 2023/09/16 16:11:24 by eflaquet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Exodus.hpp"
#include "RequestParser.hpp"

vector<t_server> serv;
RequestParser rep("eee");

t_server	choice_server()
{
	t_server tmp;

	for (vector<t_server>::iterator it = serv.begin(); it != serv.end(); it++)
	{
		if (it->listen.first == rep.get_req_head().hosts.second && it->listen.second == rep.get_req_head().hosts.first)
			tmp = *it;
	}
	return (tmp);
}

int main()
{
	string request;
	request = "\r\nTransfer-Encoding: chunked\r\n\r\n27\r\nVoici les données du premier morceau\r\n\r\n1C\r\net voici un second morceau\r\n\r\n20\r\net voici deux derniers morceaux \r\n12\r\nsans saut de ligne\r\n0\r\n\r\n";
	Server	webserver;
	int		ret;
	//Exodus pp("conf/default.conf");
	vector<t_server> context_servers;
	// names.push_back("ok");
	// names.push_back("lala");

	// allow_methods.push_back("GET");
	// allow_methods.push_back("DELETE");

	// locations.push_back(loc_1);
	// locations.push_back(loc_2)

	string tec;
	size_t pos = string::npos;
	pos = request.find("\r\nTransfer-Encoding: chunked\r\n");
	size_t posf = string::npos;
	posf = request.find("\r\n0\r\n\r\n");
	string l;
	if (pos != string::npos)
		if (posf != string::npos)
			l = request.substr(pos + 32, posf);
	cout << l ;
	while (true)
	{
		size_t p = string::npos;
		p = request.find("\r\n", 1);
		unsigned int i = 0;
		cout << p << endl;
		istringstream iss(l.substr(0, p));
		iss >> hex >> i;
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
	// 	size_t p = string::npos;
	// 	p = request.find("\r\n");
	// 	unsigned int i = 0;
	// 	istringstream iss(l.substr(0, p));
	// 	iss >> hex >> i;
	// 	l.erase(0, p + 4);
	// 	p = request.find("\r\n");

	// 	cout << l.substr(0, ) ;
	// 	l = "";
	// }



	// requestParser rep(request);
	// cout << rep.get_methods() << " " << rep.get_uri() << " " << rep.get_version() << endl;
	// cout << rep.get_req_head().host << endl;
	// context_servers.push_back(con_2);
	try {
		Exodus pp("conf/conf1.conf");
		pp.setup();
		context_servers = pp.get_server();
	}
	catch(const exception &e)
	{
		cerr << e.what() << endl;
	}

	// for (long unsigned int i = 0; i < context_servers.size(); i++)
	// {
	// 	cout << "server {" << endl;
	// 	cout << "listen " << context_servers[i].listen.first << ":" << context_servers[i].listen.second << endl;
	// 	cout << "server_name";
	// 	for (int y = 0; y < 2; y++)
	// 		cout << " " << context_servers[i].server_name[y];
	// 	cout << endl;
	// 	cout << "client_body_size " << context_servers[i].client_body_size << endl;
	// 	cout << "error_page";
	// 	for (int y = 0; y < 1; y++)
	// 		cout << " " << context_servers[i].error_pages[y].first << " " << context_servers[i].error_pages[y].second;
	// 	cout << endl;

	// 	for(long unsigned int l = 0; l < context_servers[i].location.size(); l++)
	// 	{
	// 		cout << "	location " << context_servers[i].location[l].uri <<  "{" << endl;
	// 		cout << "		allow_methods";
	// 		for (int y = 0; y < 2; y++)
	// 			cout << " " << context_servers[i].location[l].allow_methods[y];
	// 		cout << endl;
	// 		cout << "		root " << context_servers[i].location[l].root << endl;
	// 		cout << "		index " << context_servers[i].location[l].index[0] << endl;
	// 		cout << "		redir_link " << context_servers[i].location[l].redir_link << endl;
	// 		cout << "		file_location " << context_servers[i].location[l].file_location << endl;
	// 		cout << "		cgi_pass " << context_servers[i].location[l].cgi_pass << endl;
	// 		if (context_servers[i].location[l].directory_listing)
	// 			cout << "		directory_listing true" << endl;
	// 		else
	// 			cout << "		directory_listing true" << endl;
	// 		cout << "	}" << endl;
	// 	}

	// 	cout << "}" << endl;
	// }
	webserver.set_con_servs(context_servers);
	ret = webserver.serve_do_your_stuff();
	return (ret);
}