/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: afatir <afatir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/20 11:29:22 by afatir            #+#    #+#             */
/*   Updated: 2024/01/22 22:09:16 by afatir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int ac, char **av)
{
    if (ac != 2)
		{std::cout << "Usage: " << av[0] << " port number" << std::endl; return 1;}
	std::cout << "---- SERVER ----" << std::endl;
    try
    {
        Server ser(std::atoi(av[1]), 1234);
        ser.InitServer();    
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}
