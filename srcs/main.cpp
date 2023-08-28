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
#include "Exodus.hpp"

int main()
{
	Server	webserver;
	int		ret;
	Exodus pp("conf/default.conf");

	try {
		pp.get_Exodus();
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	std::cout << "Port is: " << pp.get_parss()[0].listen << std::endl;
	std::cout << "Server names are: " << pp.get_parss()[0].server_name << std::endl;
	ret = webserver.serve_do_your_stuff();
	return (ret);
}
