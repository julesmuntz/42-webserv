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

int main()
{
	//should handle signals
	//if a sigint is received
	//put it in global
	//global variable is triggered
	// if global variable is triggered
	//server stops to serve and does the necessary freeing

	Server webserver;
	int		ret;

	ret = webserver.serve_do_your_stuff();
	return (ret);
}
